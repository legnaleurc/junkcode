import edacore, re

def evaluate( data ):
	edacore.currentCircuit.evaluate( data )

def parse( line ):
	m = re.match( r'^evaluate(\s+\w+\s+[01])+$', line )
	if m == None:
		return None
	m = re.findall( r'(\w+)\s+([01])', line )
	# TODO one or two inputs
	return lambda: evaluate( [ ( a, int( b ) ) for a, b in m ] )
