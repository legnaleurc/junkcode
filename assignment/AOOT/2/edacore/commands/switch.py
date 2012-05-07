import edacore, re
from edacore import logic

def switchGate( gateName, logicType ):
	gate = edacore.currentCircuit.getGate( gateName )
	gate.logic = logic.createLogic( logicType )
	print 'The type of the gate "{0}" has become {1}'.format( gateName, logicType )

def parse( line ):
	m = re.match( r'^switch\s+gate\s+(\w+)\s+(AND|NOT|OR|NAND|NOR)$', line )
	if m == None:
		return None
	return lambda: switchGate( m.group( 1 ), m.group( 2 ) )
