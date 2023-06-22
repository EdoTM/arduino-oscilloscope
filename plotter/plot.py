import collections

import numpy as np
import pyqtgraph as pg
import serial
from PyQt6 import QtWidgets
from pyqtgraph.Qt import QtCore


class Plotter:
    def __init__(self, sampleinterval=0.1, timewindow=10., size=(600, 350)):
        # Data stuff
        self._interval = int(sampleinterval * 1000)
        self._bufsize = int(timewindow / sampleinterval)
        self.buffer = collections.deque([0.0] * self._bufsize, maxlen=self._bufsize)
        self.x = np.linspace(-timewindow, 0.0, self._bufsize)
        self.y = np.zeros(self._bufsize, dtype=float)

        # PyQtGraph stuff
        self.win = pg.GraphicsLayoutWidget(title="Oscilloscope", size=size, show=True)
        self.app = QtWidgets.QApplication([])
        self.plt = self.win.addPlot(title='Oscilloscope')
        self.plt.showGrid(x=True, y=True)
        self.plt.setLabel('left', 'Voltage', 'V')
        self.plt.setLabel('bottom', 'Time', 's')
        self.curve = self.plt.plot(self.x, self.y, pen=(255, 255, 0))

        # QTimer
        self.timer = QtCore.QTimer()
        self.timer.timeout.connect(self.updateplot)
        self.timer.start(self._interval)

    def updateplot(self):
        self.buffer.append(get_voltage())
        self.y[:] = self.buffer
        self.curve.setData(self.x, self.y)
        self.app.processEvents()

    def run(self):
        self.app.exec()


def get_voltage():
    global ser
    line = ser.readline()
    try:
        value = line.decode('utf-8').strip()
    except UnicodeDecodeError:
        return None
    if not value:
        return None
    return float(value) * 5.0 / 1023.0


ser = serial.Serial('/dev/ttyACM0', 19200, timeout=1)

if __name__ == '__main__':
    m = Plotter(sampleinterval=0.015, timewindow=10.)
    m.run()
