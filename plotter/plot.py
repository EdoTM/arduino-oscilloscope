from collections import deque

import numpy as np
import pyqtgraph as pg
import serial
from PyQt6 import QtWidgets, QtCore
from PyQt6.QtWidgets import QSizePolicy


class OscilloscopePlot(pg.PlotWidget):
    def __init__(self, parent=None):
        super().__init__(parent=parent)
        self.showGrid(x=True, y=True)
        self.setLabel('left', 'Voltage', 'V')
        self.setLabel('bottom', 'Time', 's')
        self.curve = self.plot(pen=(255, 255, 0))


class PinSelectorPanel(QtWidgets.QWidget):
    def __init__(self, pin_value_changed, parent=None):
        super().__init__(parent=parent)
        self.layout = QtWidgets.QHBoxLayout()
        self.layout.addWidget(QtWidgets.QLabel('Pin:'))
        self.pin = QtWidgets.QSpinBox()
        self.pin.valueChanged[int].connect(pin_value_changed)
        self.pin.setRange(0, 5)
        self.pin.setPrefix('A')
        self.layout.addWidget(self.pin)
        self.setLayout(self.layout)


class FrequencySelectorPanel(QtWidgets.QWidget):
    def __init__(self, initial_value, freq_value_changed, parent=None):
        super().__init__(parent=parent)
        self.layout = QtWidgets.QVBoxLayout()

        rate_label = QtWidgets.QLabel('Sample Rate:')
        rate_label.setAlignment(QtCore.Qt.AlignmentFlag.AlignCenter)
        self.layout.addWidget(QtWidgets.QLabel('Rate:'), alignment=QtCore.Qt.AlignmentFlag.AlignTop)

        inner_layout = QtWidgets.QHBoxLayout()

        self.freq = QtWidgets.QSpinBox()
        self.freq.setButtonSymbols(QtWidgets.QAbstractSpinBox.ButtonSymbols.NoButtons)
        self.freq.setRange(5, 1000)
        self.freq.setValue(initial_value)
        self.freq.setSuffix(' ms/sample')
        inner_layout.addWidget(self.freq, alignment=QtCore.Qt.AlignmentFlag.AlignTop)

        self.set_button = QtWidgets.QPushButton('Set')
        self.set_button.clicked[bool].connect(lambda: freq_value_changed(self.freq.value()))
        inner_layout.addWidget(self.set_button, alignment=QtCore.Qt.AlignmentFlag.AlignTop)

        self.layout.addLayout(inner_layout)

        self.setLayout(self.layout)


class OscilloscopeWindow(QtWidgets.QWidget):
    def __init__(self, parent=None, interval_ms=15, timewindow=10.):
        super().__init__(parent=parent)
        self.setWindowTitle('Oscilloscope')
        self.resize(1000, 350)
        self.plot = OscilloscopePlot()
        self.interval_ms = interval_ms

        self.commands = QtWidgets.QWidget()
        commands_layout = QtWidgets.QVBoxLayout()
        self.start_button = QtWidgets.QPushButton('Stop')
        self.start_button.clicked[bool].connect(self.start_button_clicked)
        commands_layout.addWidget(self.start_button, QtCore.Qt.AlignmentFlag.AlignTop)
        self.commands.setLayout(commands_layout)
        pins_panel = PinSelectorPanel(self.handle_change_pin)
        commands_layout.addWidget(pins_panel, QtCore.Qt.AlignmentFlag.AlignTop)
        freq_panel = FrequencySelectorPanel(self.interval_ms, self.handle_change_sample_rate)
        commands_layout.addWidget(freq_panel, QtCore.Qt.AlignmentFlag.AlignTop)
        spacer = QtWidgets.QSpacerItem(40, 5000, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)
        commands_layout.addItem(spacer)

        self.layout = QtWidgets.QHBoxLayout()
        self.layout.addWidget(self.plot, stretch=1)
        self.layout.addWidget(self.commands, stretch=0)

        self.setLayout(self.layout)
        self.timer = QtCore.QTimer()
        self.timer.timeout.connect(self.updateplot)

        self.timewindow = timewindow
        self.plot.setXRange(-timewindow, 0)

        self.buffer = None  # these will be initialized in change_sample_rate
        self.bufsize = None
        self.x = None
        self.y = None

        self.start = True

        self.change_sample_rate(interval_ms)

    def change_sample_rate(self, interval_ms: int):
        self.interval_ms = interval_ms
        self.bufsize = int(self.timewindow * 1000 / self.interval_ms)
        self.buffer = deque([0.0] * self.bufsize, maxlen=self.bufsize)
        self.x = np.linspace(-self.timewindow, 0.0, self.bufsize)
        self.y = np.zeros(self.bufsize)
        self.timer.stop()
        self.timer.start(self.interval_ms)

    def handle_change_sample_rate(self, rate_ms: int):
        global ser
        ser.write(bytes(f"f{rate_ms}\n", 'utf-8'))
        wait_str_from_serial('f')
        self.change_sample_rate(rate_ms)

    def handle_change_pin(self, pin: int):
        global ser
        ser.write(bytes(f"a{pin}\n", 'utf-8'))

    def start_button_clicked(self):
        self.start = not self.start
        if self.start:
            self.buffer = deque([0.0] * self.bufsize, maxlen=self.bufsize)
            self.y[:] = 0.0
            self.start_button.setText('Stop')
        else:
            self.start_button.setText('Start')

    def updateplot(self):
        if not self.start:
            return

        self.buffer.append(get_voltage_from_serial())
        self.y[:] = self.buffer
        self.plot.curve.setData(self.x, self.y)
        self.plot.setYRange(min(self.y), max(self.y))
        self.plot.enableAutoRange('xy', False)
        self.plot.update()


def get_voltage_from_serial():
    global ser
    line = ser.readline()
    try:
        value = line.decode('utf-8').strip()
    except UnicodeDecodeError:
        return None
    if not value:
        return None
    return float(value) * 5.0 / 1023.0


def setup_serial():
    wait_str_from_serial('start')


def wait_str_from_serial(s):
    global ser
    while True:
        line = ser.readline()
        try:
            value = line.decode('utf-8').strip()
        except UnicodeDecodeError:
            pass
        else:
            if not value:
                pass
            if value == s:
                break


ser = serial.Serial('/dev/ttyACM0', 19200, timeout=1)

if __name__ == '__main__':
    setup_serial()
    app = QtWidgets.QApplication([])
    win = OscilloscopeWindow()
    win.show()
    app.exec()
