from collections import deque

import numpy as np
import pyqtgraph as pg
import serial
from PyQt6 import QtWidgets, QtCore


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


class OscilloscopeWindow(QtWidgets.QWidget):
    def __init__(self, parent=None, interval_ms=15, timewindow=10.):
        super().__init__(parent=parent)
        self.setWindowTitle('Oscilloscope')
        self.resize(600, 350)
        self.plot = OscilloscopePlot()

        self.commands = QtWidgets.QWidget()
        commands_layout = QtWidgets.QVBoxLayout()
        self.start_button = QtWidgets.QPushButton('Start')
        self.start_button.clicked[bool].connect(self.start_button_clicked)
        commands_layout.addWidget(self.start_button)
        self.commands.setLayout(commands_layout)
        pins_panel = PinSelectorPanel(self.change_pin)
        commands_layout.addWidget(pins_panel)

        self.layout = QtWidgets.QHBoxLayout()
        self.layout.addWidget(self.plot)
        self.layout.addWidget(self.commands)

        self.setLayout(self.layout)
        self.timer = QtCore.QTimer()
        self.timer.timeout.connect(self.updateplot)
        self.timer.start(interval_ms)

        self.timewindow = timewindow
        self.plot.setXRange(-timewindow, 0)
        self.bufsize = int(timewindow * 1000 / interval_ms)
        self.buffer = deque([0.0] * self.bufsize, maxlen=self.bufsize)
        self.x = np.linspace(-timewindow, 0.0, self.bufsize)
        self.y = np.zeros(self.bufsize)
        self.start = True

    def change_pin(self, pin):
        global ser
        ser.write(bytes(f"{pin}\n", 'utf-8'))

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
            if value == 'start':
                break


ser = serial.Serial('/dev/ttyACM0', 19200, timeout=1)

if __name__ == '__main__':
    setup_serial()
    app = QtWidgets.QApplication([])
    win = OscilloscopeWindow()
    win.show()
    app.exec()
