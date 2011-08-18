#! /usr/bin/env python
#-*- coding: utf-8 -*-

from PySide import QtNetwork, QtCore

class Socket( QtCore.QObject ):
	readyRead = QtCore.Signal()
	connected = QtCore.Signal()
	disconnected = QtCore.Signal()

	def __init__( self, parent = None, socket = None ):
		QtCore.QObject.__init__( self, parent )

		if socket is not None:
			self.__socket = socket
		else:
			self.__socket = QtNetwork.QTcpSocket( self )
		self.__socket.connected.connect( self.connected )
		self.__socket.disconnected.connect( self.disconnected )
		self.__socket.readyRead.connect( self.__onReadyRead )

		self.__blockSize = 0L
		self.__buffer = QtCore.QByteArray()
		self.__din = QtCore.QDataStream( self.__buffer, QtCore.QIODevice.ReadOnly )

		self.__queue = []

	def connectToHost( self, host, port ):
		return self.__socket.connectToHost( host, port )

	def disconnectFromHost( self ):
		self.__socket.disconnectFromHost()

	def read( self ):
		if len( self.__queue ) > 0:
			packet = self.__queue[0]
			del self.__queue[0]
			return packet
		else:
			return ( None, None )

	def waitForConnected( self, msecs = 30000 ):
		return self.__socket.waitForConnected( msecs )

	def waitForDisconnected( self, msecs = 30000 ):
		return self.__socket.waitForDisconnected( msecs )

	def waitForReadyRead( self, msecs = 30000 ):
		wait = QtCore.QEventLoop()
		self.readyRead.connect( wait.quit )
		if msecs > 0:
			timer = QtCore.QTimer()
			timer.setSingleShot( True )
			timer.setInterval( msecs )
			self.readyRead.connect( timer.stop )
			timer.timeout.connect( wait.quit )
		wait.exec_()
		return len( self.__queue ) > 0

	def write( self, command, data ):
		block = QtCore.QByteArray()
		dout = QtCore.QDataStream( block, QtCore.QIODevice.WriteOnly )
		dout.writeInt64( 0L )
		dout.writeQString( command )
		dout.writeQVariant( data )
		dout.device().seek( 0 )
		dout.writeInt64( long( len( block ) - 8 ) )

		self.__socket.write( block )

	def __onReadyRead( self ):
		self.__buffer.append( self.__socket.readAll() )

		while self.__readBlockSize():
			command = self.__din.readQString()
			data = self.__din.readQVariant()
			self.__din.device().seek( 0 )
			self.__buffer.remove( 0, self.__blockSize )
			self.__blockSize = 0L
			self.__queue.append( ( command, data ) )
			self.readyRead.emit()

	def __readBlockSize( self ):
		if self.__blockSize <= 0L and self.__buffer.size() >= 8:
			self.__blockSize = self.__din.readInt64()
			self.__din.device().seek( 0 )
			self.__buffer.remove( 0, 8 )
		return self.__blockSize > 0L and self.__buffer.size() >= self.__blockSize

class Server( QtCore.QObject ):
	newConnection = QtCore.Signal( Socket )

	def __init__( self, parent = None ):
		QtCore.QObject.__init__( self, parent )

		self.server = QtNetwork.QTcpServer( self )
		self.server.newConnection.connect( self.__onNewConnection )

	def close( self ):
		self.server.close()

	def listen( self, host, port ):
		return self.server.listen( host, port )

	def __onNewConnection( self ):
		while self.server.hasPendingConnections():
			socket = Socket( socket = self.server.nextPendingConnection(), parent = self )
			self.newConnection.emit( socket )
