#! /usr/bin/env python
#-*- coding: utf-8 -*-

from PySide import QtGui
import dtv
import sys

a = QtGui.QApplication( sys.argv )

view = dtv.Viewer()
view.show()

a.exec_()
