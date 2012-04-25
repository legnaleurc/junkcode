__factory__ = []

def __init__():
	import commands, os
	plugins = map( lambda x: os.path.splitext( x )[0], filter( lambda x: x.endswith( '.py' ) and x != '__init__.py' and x != 'tests.py', os.listdir( os.path.dirname( commands.__file__ ) ) ) )
	for plugin in plugins:
		module = None
		if __name__ == '__main__':
			module = __import__( plugin )
		else:
			module = __import__( '{0}.{1}'.format( commands.__name__, plugin ), fromlist = [ 'parse' ] )
		__factory__.append( module.parse )

def parse( line ):
	for cmd in __factory__:
		data = cmd( line )
		if data != None:
			return data
	return None

__all__ = [ 'parse' ]

__init__()
