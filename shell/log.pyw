#! /usr/bin/env python
#-*- coding: utf-8 -*-

from PyQt4.QtGui import *
from PyQt4.QtCore import *
import sys

def main( args ):
	if args == None:
		args = sys.argv

	app = QApplication( args )

	widget = QWidget()
	mainBox = QHBoxLayout( widget )
	widget.setLayout( widget )

	textarea = QTextEdit( widget )
	mainBox.addWidget( textarea )

	close = QPushButton( 'Close', widget )
	mainBox.addWidget( close )
	close.setDisabled( True )
	QObject.connect( close, SIGNAL( 'clicked()' ), app, SLOT( 'quit()' ) )

	line = sys.stdin.readline()
	while line:
		textarea.append( line )
		line = sys.stdin.readline()

	close.setEnabled( True )

	return app.exec_()

if __name__ == '__main__':
	sys.exit( main() )
