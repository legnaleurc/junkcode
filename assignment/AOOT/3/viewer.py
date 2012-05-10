#! /usr/bin/env python
#-*- coding: utf-8 -*-

from PySide import QtGui
import dtv
import sys

class MainWindow( QtGui.QMainWindow ):

	def __init__( self ):
		super( MainWindow, self ).__init__()

		from forms.viewer import Ui_MainWindow
		self.ui = Ui_MainWindow()
		self.ui.setupUi( self )

		self.ui.action_Open.triggered.connect( self.__onOpen )

	def __onOpen( self ):
		fileName, selectedFilter = QtGui.QFileDialog.getOpenFileName( self )
		if len( fileName ) == 0:
			return
		self.ui.graphicsView.openDtv( fileName )

a = QtGui.QApplication( sys.argv )

view = MainWindow()
view.show()

a.exec_()
