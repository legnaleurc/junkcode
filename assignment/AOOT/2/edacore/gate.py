import logic

class Gate:
	def __init__( self, type_, name ):
		self.logic = logic.createLogic( type_ )
		self.name = name
		self.input1 = -1
		self.input2 = -1
		self.outputLines = []

	def __str__( self ):
		return '{0} {1}'.format( self.name, self.logic.type_ )

	def __call__( self ):
		if not self.logic.canEvaluate( self.input1, self.input2 ):
			return

		output = self.logic( self.input1, self.input2 )

		for line in self.outputLines:
			line.value = output
		for line in self.outputLines:
			line()
