#! /usr/bin/env python
#-*- coding: utf-8 -*-

import sys
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4.QtWebKit import *

def main( args = None ):
	if args is None:
		args = sys.argv
	
	app = QApplication( args )
	
	center = QWidget()
	layout = QVBoxLayout( center )
	center.setLayout( layout )
	
	url = QLineEdit( center )
	layout.addWidget( url )
	
	web = QWebView( center )
	web.load( QUrl( 'about:config' ) )
	layout.addWidget( web )
	
	QObject.connect( url, SIGNAL( 'returnPressed()' ), lambda: web.load( QUrl( url.text() ) ) )
	
	center.show()
	
	return app.exec_()

if __name__ == '__main__':
	sys.exit( main() )
