from PyQt5 import QtWidgets, uic

from .ui import Ui_MainWindow


class MainWindow(QtWidgets.QMainWindow):

    def __init__(self, parent=None):
        super(MainWindow, self).__init__(parent)

        self._ui = Ui_MainWindow()
        self._ui.setupUi(self)
