import edacore, re

def listAll():
	edacore.currentCircuit.listAll()

def parse( line ):
	m = re.match( r'^list$', line )
	if m == None:
		return None
	return listAll
