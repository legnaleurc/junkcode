import sys

from PyQt5 import QtWidgets

from . import widgets


def main(args=None):
    if args is None:
        args = sys.argv

    app = QtWidgets.QApplication(args)

    window = widgets.MainWindow()
    window.show()

    return app.exec_()
