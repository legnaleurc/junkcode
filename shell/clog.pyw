#! /usr/bin/env python
#-*- coding: utf-8 -*-

from PyQt4.QtGui import *
from PyQt4.QtCore import *
from subprocess import Popen, PIPE
import sys, os

class Listener( QThread ):
	def __init__( self, stream, parent = None ):
		QThread.__init__( self, parent )
		self.__stream = stream

	def run( self ):
		line = self.__stream.readline()
		while line:
			self.emit( SIGNAL( 'readed( const QString & )' ), QString.fromLocal8Bit( line ) )
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

	def save( self ):
		filePath = QFileDialog.getSaveFileName( self, 'Save log file', QDir.homePath() )
		if filePath:
			fout = QFile( filePath )
			if not fout.open( QIODevice.WriteOnly | QIODevice.Text ):
				return
			fout.write( self.__screen.toPlainText().toUtf8() )
			fout.close()

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

class MsgDlg( QWidget ):
	def __init__( self, args, parent = None ):
		QWidget.__init__( self, parent )

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
			cmd = QString.fromLocal8Bit( sys.stdin.read() ).simplified()
		else:
			cmd = QStringList( map( lambda s: '"'+s+'"', args ) ).join( ' ' )
		self.setWindowTitle( u'Clog -- ' + cmd )

		layout = QVBoxLayout( self )
		self.setLayout( layout )

		self.__pages = QTabWidget( self )
		layout.addWidget( self.__pages )

		self.__buttons = QDialogButtonBox( QDialogButtonBox.Save | QDialogButtonBox.Close, Qt.Horizontal, self )
		layout.addWidget( self.__buttons )
		self.__buttons.setDisabled( True )
		QObject.connect( self.__buttons.button( QDialogButtonBox.Close ), SIGNAL( 'clicked()' ), self, SLOT( 'close()' ) )
		QObject.connect( self.__buttons.button( QDialogButtonBox.Save ), SIGNAL( 'clicked()' ), lambda: self.__pages.currentWidget().save() )

		pipes = Popen( unicode( cmd ), shell = True, stdin = PIPE, stdout = PIPE, stderr = PIPE )
		self.__addPage( InputArea( pipes.stdin, self ), 'stdin' )
		self.__addPage( MsgArea( pipes.stdout, self ), 'stdout' )
		self.__addPage( MsgArea( pipes.stderr, self ), 'stderr' )

	def __addPage( self, textArea, title ):
		self.__pages.addTab( textArea, title )
		QObject.connect( textArea, SIGNAL( 'finished' ), lambda: self.__buttons.setEnabled( True ) )

	def setCurrentIndex( self, index ):
		self.__pages.setCurrentIndex( index )

def main( args = None ):
	if args == None:
		args = sys.argv

	app = QApplication( args )
	# hack arguments on Windows. DAMN THE WINDOWS!
	args = QApplication.arguments()[2:] if os.name == 'nt' else QApplication.arguments()[1:]

	widget = MsgDlg( args )
	widget.setCurrentIndex( 2 )
	widget.show()

	return app.exec_()

if __name__ == '__main__':
	sys.exit( main() )
