#! /usr/bin/env python
#-*- coding: utf-8 -*-

from PyQt4.QtGui import *
from PyQt4.QtCore import *
import sys, os, locale

class Listener( QThread ):
	def __init__( self, stream, parent = None ):
		QThread.__init__( self, parent )
		self.__stream = stream

	def run( self ):
		line = self.__stream.readline()
		while line:
			self.emit( SIGNAL( 'readed( const QString & )' ), unicode( line, locale.getpreferredencoding() ) )
			line = self.__stream.readline()

class InputArea( QWidget ):
	def __init__( self, stream, parent = None ):
		QWidget.__init__( self, parent )
		self.__stream = stream

		layout = QVBoxLayout( self )
		self.setLayout( layout )

		self.__screen = QTextEdit( self )
		self.__screen.setReadOnly( True )
		self.__screen.setFontFamily( 'monospace' )
		layout.addWidget( self.__screen )

		inputBox = QHBoxLayout()
		layout.addLayout( inputBox )

		self.__input = QLineEdit( self )
		self.connect( self.__input, SIGNAL( 'returnPressed()' ), self.read )
		inputBox.addWidget( self.__input )

		eot = QPushButton( 'EOT', self )
		self.connect( eot, SIGNAL( 'clicked()' ), self.eot )
		inputBox.addWidget( eot )

	def read( self ):
		line = self.__input.text() + '\n'
		self.__input.clear()
		self.__screen.insertPlainText( line )
		self.__screen.moveCursor( QTextCursor.End )
		self.__stream.write( line.toLocal8Bit() )
		self.__stream.flush()

	def eot( self ):
		self.__stream.close()
		self.emit( SIGNAL( 'finished' ) )

class MsgArea( QTextEdit ):
	def __init__( self, stream, parent = None ):
		QTextEdit.__init__( self, parent )

		self.setReadOnly( True )
		self.setFontFamily( 'monospace' )

		listener = Listener( stream, self )
		QObject.connect( listener, SIGNAL( 'readed( const QString & )' ), self.write )
		QObject.connect( listener, SIGNAL( 'finished()' ), lambda: self.emit( SIGNAL( 'finished' ) ) )

		listener.start()

	def write( self, line ):
		self.insertPlainText( line )
		self.moveCursor( QTextCursor.End )

	def save( self ):
		filePath = QFileDialog.getSaveFileName( self, 'Save log file', QDir.homePath() )
		if filePath:
			fout = QFile( filePath )
			if not fout.open( QIODevice.WriteOnly | QIODevice.Text ):
				return
			fout.write( self.toPlainText().toUtf8() )
			fout.close()

class MsgDlg( QTabWidget ):
	def __init__( self, args, parent = None ):
		QTabWidget.__init__( self, parent )

		#FIXME: should be this:
		#self.setWindowFlags( self.windowFlags() | Qt.CustomizeWindowHint ^ Qt.WindowCloseButtonHint )
		self.setWindowFlags( Qt.Window | Qt.CustomizeWindowHint | Qt.WindowTitleHint | Qt.WindowMinMaxButtonsHint )

		# handle arguments
		if len( args ) == 0:
			cmd, ok = QInputDialog.getText( self, 'Enter command', 'Command:' )
			if not ok:
				self.close()
			cmd = cmd.simplified()
		elif args[0] == '-':
			cmd = QString( sys.stdin.read().decode( locale.getpreferredencoding() ) ).simplified()
		else:
			cmd = QStringList( map( lambda s: '"'+s+'"', args ) ).join( ' ' )
		self.setWindowTitle( u'Clog -- ' + cmd )

		pipes = os.popen3( unicode( cmd ) )

		self.__setPage( InputArea( pipes[0], self ), 'stdin' )
		self.__setPage( MsgArea( pipes[1], self ), 'stdout' )
		self.__setPage( MsgArea( pipes[2], self ), 'stderr' )

	def __setPage( self, textArea, title ):
		widget = QWidget( self )
		layout = QVBoxLayout( widget )
		widget.setLayout( layout )
		self.addTab( widget, title )

		layout.addWidget( textArea )

		buttons = QDialogButtonBox( QDialogButtonBox.Save | QDialogButtonBox.Close, Qt.Horizontal, self )
		layout.addWidget( buttons )
		buttons.setDisabled( True )
		QObject.connect( buttons.button( QDialogButtonBox.Close ), SIGNAL( 'clicked()' ), self, SLOT( 'close()' ) )
		QObject.connect( buttons.button( QDialogButtonBox.Save ), SIGNAL( 'clicked()' ), lambda: textArea.save() )
		QObject.connect( textArea, SIGNAL( 'finished' ), lambda: buttons.setEnabled( True ) )

def main( args = None ):
	if args == None:
		args = sys.argv

	app = QApplication( args )

	widget = MsgDlg( QApplication.arguments()[1:] )
	widget.setCurrentIndex( 2 )
	widget.show()

	return app.exec_()

if __name__ == '__main__':
	sys.exit( main() )
