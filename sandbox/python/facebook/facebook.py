#! /usr/bin/env python
#-*- coding: utf-8 -*-

from PySide import QtGui, QtWebKit, QtCore, QtNetwork
import sys

import private

__PRIVATE__ = private.load()
APP_ID = __PRIVATE__['APP_ID']
APP_SECRET = __PRIVATE__['APP_SECRET']
ACCESS_TOKEN = None
EXPIRES = None

app = QtGui.QApplication( sys.argv )

view = QtWebKit.QWebView()
view.show()

url = QtCore.QUrl( u'https://www.facebook.com/dialog/oauth' )
url.addQueryItem( u'client_id', APP_ID )
url.addQueryItem( u'redirect_uri', u'https://www.facebook.com/connect/login_success.html' )
url.addQueryItem( u'scope', u'publish_stream' )

def onUrlChanged( url ):
	if url.path() == u'/connect/login_success.html':
		if url.hasQueryItem( u'code' ):
			# success
			code = url.queryItemValue( u'code' )
			requestAccessToken( code )
		else:
			# failed
			pass

def requestAccessToken( code ):
	url = QtCore.QUrl( u'https://graph.facebook.com/oauth/access_token' )
	url.addQueryItem( u'client_id', APP_ID )
	url.addQueryItem( u'redirect_uri', u'https://www.facebook.com/connect/login_success.html' )
	url.addQueryItem( u'client_secret', APP_SECRET )
	url.addQueryItem( u'code', code )
	view.load( url )

def onLoadFinished( ok ):
	if not ok:
		pass
	url = view.url()
#	print url.toString()
	if url.path() == u'/oauth/access_token':
		response = view.page().mainFrame().toPlainText()
		url = QtCore.QUrl.fromUserInput( u'http://a.b/?{0}'.format( response ) )
		if url.hasQueryItem( u'access_token' ):
			# store access_token
			ACCESS_TOKEN = url.queryItemValue( u'access_token' )
			EXPIRES = url.queryItemValue( u'expires' )
			greeting( ACCESS_TOKEN )

def greeting( accessToken ):
	url = QtCore.QUrl( u'https://graph.facebook.com/me/feed' )
	url.addQueryItem( u'access_token', accessToken )
	url.addQueryItem( u'message', u'Powered by PySide.' )
	print url.toString()
	nasm = QtNetwork.QNetworkAccessManager()
	reply = nasm.post( QtNetwork.QNetworkRequest( url ), url.encodedQuery() )
	wait = QtCore.QEventLoop()
	reply.finished.connect( wait.quit )
	wait.exec_()
	print unicode( reply.readAll() )

view.urlChanged.connect( onUrlChanged )
view.loadFinished.connect( onLoadFinished )

view.load( url )

app.exec_()
