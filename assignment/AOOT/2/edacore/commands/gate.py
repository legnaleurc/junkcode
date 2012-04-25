import edacore, re

def addGate( gateType, gateName ):
	# TODO factory
	from edacore.gate import Gate
	gate = Gate( gateType, gateName )
	edacore.currentCircuit.addGate( gate )
	print 'one {0}-gate called "{1}" has been added to circuit {2}'.format( gateType, gateName, edacore.currentCircuit.name )

def parse( line ):
	m = re.match( r'^add\s+gate\s+(AND|NOT|OR|NAND|NOR)\s+(\w+)$', line )
	if m == None:
		return None
	return lambda: addGate( m.group( 1 ), m.group( 2 ) )
