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

		self.__textArea = QTextEdit( self )
		layout.addWidget( self.__textArea )
		self.__textArea.setReadOnly( True )
		self.__textArea.setFontFamily( 'monospace' )
		def helper( line ):
			self.__textArea.insertPlainText( line )
			self.__textArea.ensureCursorVisible()

		self.__monitor = Monitor( self )
		QObject.connect( self.__monitor, SIGNAL( 'readed( const QString & )' ), helper )

		buttons = QDialogButtonBox( QDialogButtonBox.Save | QDialogButtonBox.Close, Qt.Horizontal, self )
		layout.addWidget( buttons )
		buttons.setDisabled( True )
		QObject.connect( self.__monitor, SIGNAL( 'finished()' ), lambda: buttons.setEnabled( True ) )
		QObject.connect( buttons.button( QDialogButtonBox.Close ), SIGNAL( 'clicked()' ), self, SLOT( 'close()' ) )
		QObject.connect( buttons.button( QDialogButtonBox.Save ), SIGNAL( 'clicked()' ), self.save )

	def monitor( self ):
		self.__monitor.start()

	def save( self ):
		filePath = QFileDialog.getSaveFileName( self, 'Save log file', QDir.homePath() )
		if filePath:
			fout = QFile( filePath )
			if not fout.open( QIODevice.WriteOnly | QIODevice.Text ):
				return
			fout.write( self.__textArea.toPlainText().toUtf8() )
			fout.close()

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
