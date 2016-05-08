import sys

from PyQt5 import QtWidgets

from . import widgets
from . import models


def main(args=None):
    if args is None:
        args = sys.argv

    app = QtWidgets.QApplication(args)

    window = widgets.MainWindow()
    window.show()

    model = models.ACDModel()
    window.setModel(model)

    return app.exec_()
