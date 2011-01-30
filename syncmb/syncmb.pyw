#! /usr/bin/env python
#-*- coding: utf-8 -*-

import tweepy, plurklib, conf, sys, os
from PySide import QtCore, QtGui

class MainWindow( QtGui.QMainWindow ):
	def __init__( self ):
		super( MainWindow, self ).__init__( None )
		from ui.mainwindow import Ui_MainWindow
		self.ui = Ui_MainWindow()
		self.ui.setupUi( self )
		
		self.connect( self.ui.action_Submit, QtCore.SIGNAL( 'triggered()' ), self.submit )
		
		#setup twitter
		auth = tweepy.OAuthHandler( conf.api['twitter']['key'], conf.api['twitter']['secret'] )
		if not conf.user.has_key( 'twitter' ):
			PIN = OAuthDialog.getPIN( auth.get_authorization_url() )
			auth.get_access_token( PIN )
			conf.user['twitter'] = { 'key': auth.access_token.key, 'secret': auth.access_token.secret }
		auth.set_access_token( conf.user['twitter']['key'], conf.user['twitter']['secret'] )
		self.tweet = tweepy.API( auth )
		
		#setup plurk
		self.plurk = plurklib.PlurkAPI( conf.api['plurk']['key'] )
		if not conf.user.has_key( 'plurk' ):
			username, password = PlurkDialog.getUserAccount()
			conf.user['plurk'] = { 'username': username, 'password': password }
		self.plurk.login( conf.user['plurk']['username'], conf.user['plurk']['password'] )
	
	def __del__( self ):
		self.plurk.logout()
	
	def submit( self ):
		msg = self.ui.plainTextEdit.toPlainText()
		try:
			self.tweet.update_status( msg )
		except tweepy.error.TweepError as e:
			self.ui.statusbar.showMessage( unicode( e ) )
		try:
			self.plurk.plurkAdd( msg.encode( 'utf-8' ) )
		except Exception as e:
			self.ui.statusbar.showMessage( unicode( e ) )
		self.ui.plainTextEdit.clear()

class OAuthDialog( QtGui.QDialog ):
	def __init__( self, parent ):
		super( OAuthDialog, self ).__init__( parent )
		from ui.oauthdialog import Ui_OAuthDialog
		self.ui = Ui_OAuthDialog()
		self.ui.setupUi( self )
	
	@staticmethod
	def getPIN( url ):
		this = OAuthDialog( None )
		this.ui.label.setText( this.ui.label.text() % url )
		role = this.exec_()
		if role == QtGui.QDialog.Accepted:
			PIN = this.ui.lineEdit.text()
			del this
			return PIN
		else:
			del this
			return ''

class PlurkDialog( QtGui.QDialog ):
	def __init__( self, parent ):
		super( PlurkDialog, self ).__init__( parent )
		from ui.plurkdialog import Ui_PlurkDialog
		self.ui = Ui_PlurkDialog()
		self.ui.setupUi( self )
	
	@staticmethod
	def getUserAccount():
		this = PlurkDialog( None )
		role = this.exec_()
		if role == QtGui.QDialog.Accepted:
			userName = this.ui.userName.text()
			password = this.ui.password.text()
			del this
			return userName, password
		else:
			del this
			return '', ''

def main( args ):
	app = QtGui.QApplication( args )
		
	window = MainWindow()
	window.show()
	
	return app.exec_()

if __name__ == '__main__':
	sys.exit( main( sys.argv ) )
