from PySide import QtGui, QtWebKit, QtNetwork, QtCore

import private

__PRIVATE__ = private.load()
APP_ID = __PRIVATE__['APP_ID']
APP_SECRET = __PRIVATE__['APP_SECRET']

class FacebookWidget( QtGui.QWidget ):
	def __init__( self ):
		QtGui.QWidget.__init__( self )

		self.view = QtWebKit.QWebView( self )
		self.view.setWindowFlags( QtCore.Qt.Dialog )
		self.input = QtGui.QLineEdit( self )
		self.output = QtGui.QTextBrowser( self )
		self.nasm = QtNetwork.QNetworkAccessManager( self )
		self.accessToken = None
		self.expires = None
		layout = QtGui.QVBoxLayout( self )
		self.setLayout( layout )
		layout.addWidget( self.input )
		layout.addWidget( self.output )

		self.view.loadFinished.connect( self.onLoadFinished )
		self.input.returnPressed.connect( self.onReturnPressed )

	def auth( self ):
		url = QtCore.QUrl( u'https://www.facebook.com/dialog/oauth' )
		url.addQueryItem( u'client_id', APP_ID )
		url.addQueryItem( u'redirect_uri', u'https://www.facebook.com/connect/login_success.html' )
		url.addQueryItem( u'scope', u'publish_stream' )
		self.view.show()
		self.view.load( url )

	def onLoadFinished( self, ok ):
		if not ok:
			pass
		url = self.view.url()
		if url.path() == u'/oauth/access_token':
			response = self.view.page().mainFrame().toPlainText()
			url = QtCore.QUrl.fromUserInput( u'http://a.b/?{0}'.format( response ) )
			if url.hasQueryItem( u'access_token' ):
				# store access_token
				self.accessToken = url.queryItemValue( u'access_token' )
				self.expires = url.queryItemValue( u'expires' )
				self.view.hide()
		elif url.path() == u'/connect/login_success.html':
			if url.hasQueryItem( u'code' ):
				# success
				code = url.queryItemValue( u'code' )
				self.requestAccessToken( code )

	def requestAccessToken( self, code ):
		url = QtCore.QUrl( u'https://graph.facebook.com/oauth/access_token' )
		url.addQueryItem( u'client_id', APP_ID )
		url.addQueryItem( u'redirect_uri', u'https://www.facebook.com/connect/login_success.html' )
		url.addQueryItem( u'client_secret', APP_SECRET )
		url.addQueryItem( u'code', code )
		self.view.load( url )

	def onReturnPressed( self ):
		url = QtCore.QUrl( u'https://graph.facebook.com/me/feed' )
		url.addQueryItem( u'access_token', self.accessToken )
		url.addQueryItem( u'message', self.input.text() )
		self.reply = self.nasm.post( QtNetwork.QNetworkRequest( url ), url.encodedQuery() )
		self.reply.finished.connect( self.onReply )

	def onReply( self ):
		response = unicode( self.reply.readAll() )
		self.output.clear()
		self.output.setPlainText( response )
		self.reply.deleteLater()
		self.reply = None
