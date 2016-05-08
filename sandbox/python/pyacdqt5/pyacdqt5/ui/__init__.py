import pkg_resources
from PyQt5 import uic


def load_ui(filename):
    full_path = pkg_resources.resource_filename(__name__, filename)
    Class, BaseClass = uic.loadUiType(full_path)
    return Class


Ui_MainWindow = load_ui('mainwindow.ui')
