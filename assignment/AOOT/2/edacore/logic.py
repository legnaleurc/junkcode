class Logic:
	def canEvaluate( self, input1, input2 ):
		return input1 != -1 and input2 != -1

class And( Logic ):
	def __init__( self ):
		self.type_ = 'AND'

	def __call__( self, input1, input2 ):
		return 1 if input1 == 1 and input2 == 1 else 0

class Nand( Logic ):
	def __init__( self ):
		self.type_ = 'NAND'

	def __call__( self, input1, input2 ):
		return 0 if input1 == 1 and input2 == 1 else 1

class Nor( Logic ):
	def __init__( self ):
		self.type_ = 'NOR'

	def __call__( self, input1, input2 ):
		return 0 if input1 == 1 or input2 == 1 else 1

class Not( Logic ):
	def __init__( self ):
		self.type_ = 'NOT'
	
	def canEvaluate( self, input1, input2 ):
		return input1 != -1

	def __call__( self, input1, input2 ):
		return 0 if input1 == 1 else 1

class Or( Logic ):
	def __init__( self ):
		self.type_ = 'OR'

	def __call__( self, input1, input2 ):
		return 1 if input1 == 1 or input2 == 1 else 0

__factory__ = {
	'AND': lambda: And(),
	'NAND': lambda: Nand(),
	'NOR': lambda: Nor(),
	'NOT': lambda: Not(),
	'OR': lambda: Or(),
}

def createLogic( type_ ):
	return __factory__[type_]()
