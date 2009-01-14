#! /usr/bin/env python
#-*- coding: utf-8 -*-

from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4.QtWebKit import *
import sys, csv

def readEngines( combo ):
	fin = csv.reader( open( "engines.csv" ) )
	for line in fin:
		combo.addItem( line[0], QVariant( line[1] ) )

def main( args = None ):
	if args is None:
		args = sys.argv

	app = QApplication( args )

	window = QMainWindow()
	window.setWindowTitle( 'QDict' )
	window.resize( 800, 600 )
	window.show()

	center = QWidget( window )
	layout = QVBoxLayout( center )
	center.setLayout( layout )
	window.setCentralWidget( center )

	barLayout = QGridLayout()
	layout.addLayout( barLayout )

	url = QLineEdit( window )
	barLayout.addWidget( url, 0, 0, 1, 8 )

	select = QComboBox( window )
	barLayout.addWidget( select, 0, 9, 1, 2 )
	readEngines( select )

	web = QWebView( window )
	layout.addWidget( web )
	web.load( QUrl( 'about:blank' ) )

	QObject.connect( url, SIGNAL( 'returnPressed()' ), lambda: web.load( QUrl( select.itemData( select.currentIndex() ).toString().arg( url.text() ) ) ) )

	status = QStatusBar( window )
	progress = QProgressBar( status )
	status.addWidget( progress )
	window.setStatusBar( status )
	progress.hide()
	status.showMessage( 'Ready' )

	QObject.connect( web, SIGNAL( 'loadStarted()' ), progress, SLOT( 'show()' ) )
	QObject.connect( web, SIGNAL( 'loadProgress( int )' ), progress, SLOT( 'setValue( int )' ) )
	QObject.connect( web, SIGNAL( 'loadFinished( bool )' ), lambda ok: ok and progress.hide() )
	#QObject.connect( web, SIGNAL( 'statusBarMessage( const QString & )' ), status, SLOT( 'showMessage( const QString & )' ) )

	#def helper( s ):
		#print s
	#QObject.connect( web, SIGNAL( 'statusBarMessage( const QString & )' ), helper )

	return app.exec_()

if __name__ == '__main__':
	sys.exit( main() )
