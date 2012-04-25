import edacore, re

def deleteCircuit():
	name = edacore.currentCircuit.name
	del edacore.currentCircuit
	print 'Your circuit {0} is deleted.'.format( name )

def parse( line ):
	m = re.match( r'^delete$', line )
	if m == None:
		return None
	return deleteCircuit
