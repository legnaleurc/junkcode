import edacore, re

def addLine( name, startGate, startPort, endGate, endPort ):
	start = edacore.currentCircuit.getGate( startGate )
	end = edacore.currentCircuit.getGate( endGate )
	if endPort == 'i1':
		endPort = 1
	elif endPort == 'i2':
		endPort = 2
	else:
		endPort = 1
	# TODO factory
	from edacore.line import Line
	line = Line( name, start, end, endPort )
	edacore.currentCircuit.addLine( line )

def parse( line ):
	m = re.match( r'^add\s+line\s+(\w+)\s+START\s+(\w+)\s+(o1|0)\s+END\s+(\w+)\s+(i1|i2|0)$', line )
	if m == None:
		return None
	return lambda: addLine( m.group( 1 ), m.group( 2 ), m.group( 3 ), m.group( 4 ), m.group( 5 ) )
