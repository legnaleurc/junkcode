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
			cmd = QString( sys.stdin.read().decode( locale.getpreferredencoding() ) ).simplified()
		else:
			cmd = QStringList( map( lambda s: '"'+s+'"', args ) ).join( ' ' )
		self.setWindowTitle( u'Clog -- ' + cmd )

		pipes = os.popen3( unicode( cmd ) )

		layout = QVBoxLayout( self )
		self.setLayout( layout )

		self.__outArea = MsgArea( pipes[1], self )
		layout.addWidget( self.__outArea )

		self.__errArea = MsgArea( pipes[2], self )
		layout.addWidget( self.__errArea )

		buttons = QDialogButtonBox( QDialogButtonBox.Close, Qt.Horizontal, self )
		layout.addWidget( buttons )
		buttons.setDisabled( True )
		QObject.connect( buttons.button( QDialogButtonBox.Close ), SIGNAL( 'clicked()' ), self, SLOT( 'close()' ) )

		save1 = buttons.addButton( 'Save stdout', QDialogButtonBox.AcceptRole )
		QObject.connect( self.__outArea, SIGNAL( 'finished' ), lambda: buttons.setEnabled( True ) )
		QObject.connect( save1, SIGNAL( 'clicked()' ), self.__outArea.save )

		save2 = buttons.addButton( 'Save stderr', QDialogButtonBox.AcceptRole )
		QObject.connect( self.__errArea, SIGNAL( 'finished' ), lambda: buttons.setEnabled( True ) )
		QObject.connect( save2, SIGNAL( 'clicked()' ), self.__errArea.save )

def main( args = None ):
	if args == None:
		args = sys.argv

	app = QApplication( args )

	widget = MsgDlg( QApplication.arguments()[1:] )
	widget.show()

	return app.exec_()

if __name__ == '__main__':
	sys.exit( main() )
