import edacore, re

def switchGate():
	pass

def parse( line ):
	m = re.match( r'^switch\s+gate\s+(\w+)\s+(AND|NOT|OR|NAND|NOR)$', line )
	if m == None:
		return None
	return switchGate
