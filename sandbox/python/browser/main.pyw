#! /usr/bin/env python
#-*- coding: utf-8 -*-

import sys
from PySide import QtCore, QtGui, QtWebKit

from ui.mainwindow import Ui_MainWindow

class MainWindow( QtGui.QMainWindow ):
	def __init__( self ):
		QtGui.QMainWindow.__init__( self, None )
		self.ui = Ui_MainWindow()
		self.ui.setupUi( self )
		
		self.connect( self.ui.uriInput, QtCore.SIGNAL( 'returnPressed()' ), self.load )
		
		self.ui.webView.load( 'about:config' )
	
	def load( self ):
		self.ui.webView.load( QtCore.QUrl.fromUserInput( self.ui.uriInput.text() ) )

def main( args = None ):
	if args is None:
		args = sys.argv
	
	app = QtGui.QApplication( args )
	
	window = MainWindow()
	window.show()
	
	return app.exec_()

if __name__ == '__main__':
	sys.exit( main() )
