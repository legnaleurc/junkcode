import edacore, re

def createCircuit( circuitName ):
	from edacore.circuit import Circuit
	edacore.currentCircuit = Circuit( circuitName )
	print 'An empty circuit called "{0}" has been created!'.format( circuitName )
	print 'Any commands you do are toward to {0}'.format( circuitName )

def parse( line ):
	m = re.match( r'^create\s+circuit\s+(\w+)$', line )
	if m == None:
		return None
	return lambda: createCircuit( m.group( 1 ) )
