#! /usr/bin/env python
#-*- coding: utf-8 -*-

import os, json
from PySide import QtCore

__API_FILE__ = '.api.json'
__USER_FILE__ = '.user.json'
__DIR__ = QtCore.QDir( os.path.dirname( os.path.realpath( __file__ ) ) )

def __load__( fileDir, fileName ):
	if( fileDir.exists( fileName ) ):
		fin = QtCore.QFile( fileDir.filePath( fileName ) )
		fin.open( QtCore.QIODevice.ReadOnly )
		data = json.loads( unicode( fin.readAll(), 'utf-8' ) )
		fin.close()
	else:
		data = None
	return data

api = __load__( __DIR__, __API_FILE__ )
user = __load__( __DIR__, __USER_FILE__ )
	
def save():
	def __save__( fileDir, fileName, data ):
		fout = QtCore.QFile( fileDir.filePath( fileName ) )
		fout.open( QtCore.QIODevice.WriteOnly )
		fout.write( json.dumps( data ) )
		fout.close()
	__save__( self.appDir, self.userFile, self.user )

__all__ = [ 'api', 'user', 'save' ]
