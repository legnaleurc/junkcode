#! /usr/bin/env python
#-*- coding: utf-8 -*-

from PyQt4.QtGui import *
from PyQt4.QtCore import *
import sys

class Monitor( QThread ):
	def __init__( self, parent = None ):
		QThread.__init__( self, parent )

	def run( self ):
		line = sys.stdin.readline()
		while line:
			self.emit( SIGNAL( 'readed( const QString & )' ), line )
			line = sys.stdin.readline()

class MsgDlg( QWidget ):
	def __init__( self, parent = None ):
		QWidget.__init__( self, parent )

		layout = QVBoxLayout( self )
		self.setLayout( layout )

		textArea = QTextEdit( self )
		layout.addWidget( textArea )
		textArea.setReadOnly( True )
		textArea.setFontFamily( 'monospace' )
		def helper( line ):
			textArea.insertPlainText( line )
			textArea.ensureCursorVisible()

		self.__monitor = Monitor( self )
		QObject.connect( self.__monitor, SIGNAL( 'readed( const QString & )' ), helper )

		close = QPushButton( 'Close', self )
		layout.addWidget( close )
		close.setDisabled( True )
		QObject.connect( close, SIGNAL( 'clicked()' ), qApp, SLOT( 'quit()' ) )
		QObject.connect( self.__monitor, SIGNAL( 'finished()' ), lambda: close.setEnabled( True ) )

	def monitor( self ):
		self.__monitor.start()

def main( args = None ):
	if args == None:
		args = sys.argv

	app = QApplication( args )

	widget = MsgDlg()
	widget.monitor()
	widget.show()

	return app.exec_()

if __name__ == '__main__':
	sys.exit( main() )
