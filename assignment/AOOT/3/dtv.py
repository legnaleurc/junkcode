class DtvFile:
	def __init__( self, filePath ):
		fin = open( filePath, 'rb' )
		chunk = fin.read( 18 )
		if chunk != '#DTV:copyright1.0#':
			raise Exception()
		chunk = fin.read( 5 )
		nImages = int( chunk )
		chunk = fin.read( 2 )
		if chunk != '@@':
			raise Exception()
		self.images = []
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
			self.images.append( Image( x, y, size, type_ ) )
		fin.close()

class Image:
	def __init__( self, x, y, size ):
		self.x = x
		self.y = y
		self.size = size
