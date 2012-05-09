from PySide import QtCore, QtGui

class Viewer( QtGui.QGraphicsView ):

	def __init__( self ):
		super( Viewer, self ).__init__()

		scene = QtGui.QGraphicsScene( self )
		scene.setSceneRect( 0.0, 0.0, 10000.0, 10000.0 )
		self.setScene( scene )
		self.images = []
		self.lastMousePos = QtCore.QPoint()

	def mouseMoveEvent( self, event ):
		if not event.buttons() & QtCore.Qt.LeftButton:
			return
		d = self.lastMousePos - event.pos()
		self.__moveScrollBars( d.x(), d.y() )
		self.lastMousePos = event.pos()

	def mousePressEvent( self, event ):
		if not event.buttons() & QtCore.Qt.LeftButton:
			return
		self.lastMousePos = event.pos()

	def openDtv( self, filePath ):
		fin = open( filePath, 'rb' )
		chunk = fin.read( 18 )
		if chunk != '#DTV:copyright1.0#':
			raise FileCorruptedError()
		chunk = fin.read( 5 )
		nImages = int( chunk )
		chunk = fin.read( 2 )
		if chunk != '@@':
			raise FileCorruptedError()
		offset = 18 + 5 + 2 + nImages * ( 5 + 5 + 9 + 3 )
		for i in xrange( nImages ):
			# read descriptor
			chunk = fin.read( 5 )
			x = int( chunk )
			chunk = fin.read( 5 )
			y = int( chunk )
			chunk = fin.read( 9 )
			size = int( chunk )
			chunk = fin.read( 3 )
			type_ = chunk
			item = ProxyItem( filePath, offset, size, type_ )
			self.scene().addItem( item )
			item.setPos( x, y )
			self.images.append( item )
			offset = offset + size
		fin.close()

	def __moveScrollBars( self, x, y ):
		self.horizontalScrollBar().setValue( self.horizontalScrollBar().value() + x )
		self.verticalScrollBar().setValue( self.verticalScrollBar().value() + y )

class ProxyItem( QtGui.QGraphicsItem ):

	def __init__( self, filePath, offset, size, type_ ):
		super( ProxyItem, self ).__init__()

		self.filePath = filePath
		self.offset = offset
		self.size = size
		self.boundry = self.__getBoundry( type_ )
		self.pixmap = QtGui.QPixmap()

	def boundingRect( self ):
		return QtCore.QRectF( self.pos(), QtCore.QSizeF( self.boundry[0], self.boundry[1] ) )

	def paint( self, painter, option, widget ):
		if self.pixmap.isNull():
			self.__preparePixmap()
			painter.drawRect( self.boundingRect().toRect() )
		else:
			painter.drawPixmap( self.boundingRect().toRect(), self.pixmap )

	def __preparePixmap( self ):
		# TODO open a thread
		fin = open( self.filePath, 'rb' )
		fin.seek( self.offset )
		chunk = fin.read( self.size )
		fin.close()
		chunk = QtCore.QByteArray( chunk )
		buffer_ = QtCore.QBuffer( chunk )
		reader = QtGui.QImageReader( buffer_ )
		self.pixmap = QtGui.QPixmap.fromImageReader( reader )

	def __getBoundry( self, type_ ):
		import struct
		if type_ == 'PNG':
			fin = open( self.filePath, 'rb' )
			fin.seek( self.offset + 1 )
			chunk = fin.read( 3 )
			if chunk != 'PNG':
				raise FileCorruptedError()
			fin.seek( self.offset + 8 + 4 )
			chunk = fin.read( 4 )
			if chunk != 'IHDR':
				raise FileCorruptedError()
			fin.seek( self.offset + 8 + 8 )
			chunk = fin.read( 4 )
			width = struct.unpack( '>i', chunk )[0]
			chunk = fin.read( 4 )
			height = struct.unpack( '>i', chunk )[0]
			fin.close()
			return ( width, height )
		if type_ == 'JPG':
			fin = open( self.filePath, 'rb' )
			fin.seek( self.offset )
			chunk = fin.read( 2 )
			if chunk != '\xFF\xD8':
				raise FileCorruptedError()
			while True:
				chunk = fin.read( 2 )
				if len( chunk ) < 2:
					break
				if chunk != '\xFF\xC0':
					# skip
					chunk = fin.read( 2 )
					length = struct.unpack( '>h', chunk )[0]
					# length contains size field itself
					fin.seek( length - 2, 1 )
				else:
					# skip length byte and depth byte
					fin.seek( 2 + 1, 1 )
					chunk = fin.read( 2 )
					height = struct.unpack( '>h', chunk )[0]
					chunk = fin.read( 2 )
					width = struct.unpack( '>h', chunk )[0]
					return ( width, height )
			return ( 0.0, 0.0 )
		else:
			return ( 0.0, 0.0 )

class FileCorruptedError( Exception ):
	pass
