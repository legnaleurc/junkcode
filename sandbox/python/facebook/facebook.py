#! /usr/bin/env python
#-*- coding: utf-8 -*-

from PySide import QtGui
import sys

import widget

app = QtGui.QApplication( sys.argv )

view = widget.FacebookWidget()
view.show()
view.auth()

app.exec_()
