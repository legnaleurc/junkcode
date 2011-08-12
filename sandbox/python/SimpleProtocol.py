#! /usr/bin/env python
#-*- coding: utf-8 -*-

from PySide import QtNetwork, QtCore

class Socket( QtCore.QObject ):
	messageReceived = QtCore.Signal( unicode, object )
	connected = QtCore.Signal()
	disconnected = QtCore.Signal()

	def __init__( self, parent = None, socket = None ):
		QtCore.QObject.__init__( self, parent )

		if socket is not None:
			self.socket = socket
		else:
			self.socket = QtNetwork.QTcpSocket( self )
		self.socket.connected.connect( self.connected )
		self.socket.disconnected.connect( self.disconnected )
		self.socket.readyRead.connect( self.onReadyRead )

		self.blockSize = 0L
		self.buffer = QtCore.QByteArray()
		self.din = QtCore.QDataStream( self.buffer, QtCore.QIODevice.ReadOnly )

	def connectToHost( self, host, port ):
		return self.socket.connectToHost( host, port )

	def disconnectFromHost( self ):
		self.socket.disconnectFromHost();

	def sendMessage( self, command, data ):
		block = QtCore.QByteArray()
		dout = QtCore.QDataStream( block, QtCore.QIODevice.WriteOnly )
		dout.writeInt64( 0L )
		dout.writeQString( command )
		dout.writeQVariant( data )
		dout.device().seek( 0 )
		dout.writeInt64( long( len( block ) - 8 ) )

		self.socket.write( block )

	def onReadyRead( self ):
		self.buffer.append( self.socket.readAll() )

		while self.readBlockSize():
			command = self.din.readQString()
			data = self.din.readQVariant()
			self.din.device().seek( 0 )
			self.buffer.remove( 0, self.blockSize )
			self.blockSize = 0L
			self.messageReceived.emit( command, data )

	def readBlockSize( self ):
		if self.blockSize <= 0L and self.buffer.size() >= 8:
			self.blockSize = self.din.readInt64()
			self.din.device().seek( 0 )
			self.buffer.remove( 0, 8 )
		return self.blockSize > 0L and self.buffer.size() >= self.blockSize

class Server( QtCore.QObject ):
	newConnection = QtCore.Signal( Socket )

	def __init__( self, parent = None ):
		QtCore.QObject.__init__( self, parent )

		self.server = QtNetwork.QTcpServer( self )
		self.server.newConnection.connect( self.onNewConnection )

	def close( self ):
		self.server.close()

	def listen( self, host, port ):
		return self.server.listen( host, port )

	def onNewConnection( self ):
		while self.server.hasPendingConnections():
			socket = Socket( socket = self.server.nextPendingConnection(), parent = self )
			self.newConnection.emit( socket )
