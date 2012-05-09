from PySide import QtGui

class Viewer( QtGui.QGraphicsView ):

	def __init__( self ):
		super( Viewer, self ).__init__()

		scene = QtGui.QGraphicsScene( self )
		scene.setSceneRect( 0.0, 0.0, 10000.0, 10000.0 )
		self.setScene( scene )
		self.images = []

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

class ProxyItem( QtGui.QGraphicsItem ):
	def __init__( self, filePath, offset, size, type_ ):
		self.filePath = filePath
		self.offset = offset
		self.size = size
		self.boundry = self.__getBoundry( type_ )

	def __getBoundry( self, type_ ):
		import struct
		if type_ == 'PNG':
			fin = open( filePath, 'rb' )
			fin.seek( 1 )
			chunk = fin.read( 3 )
			if chunk != 'PNG':
				raise FileCorruptedError()
			fin.seek( 8 + 4 )
			chunk = fin.read( 4 )
			if chunk != 'IHDR':
				raise FileCorruptedError()
			fin.seek( 8 + 8 )
			chunk = fin.read( 4 )
			width = struct.unpack( '<i', chunk )
			chunk = fin.read( 4 )
			height = struct.unpack( '<i', chunk )
			fin.close()
			return ( width, height )
		else:
			return ( 0.0, 0.0 )

class FileCorruptedError( Exception ):
	pass
