import sys

from PySide2 import QtWidgets

from .models import DriveModel


app = QtWidgets.QApplication(sys.argv)

model = DriveModel()

viewer = QtWidgets.QTreeView()
viewer.setModel(model)
viewer.show()
model.setRootPath()

app.exec_()
