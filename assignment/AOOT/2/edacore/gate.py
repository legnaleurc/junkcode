class Gate:
	def __init__( self, type_, name ):
		self.type_ = type_
		self.name = name
		self.input1 = -1
		self.input2 = -1
		self.outputLines = []

	def __str__( self ):
		return '{0} {1}'.format( self.name, self.type_ )

	def __call__( self ):
#		print 'GATE {0} {1} {2}'.format( self.name, self.input1, self.input2 )
		if self.input1  == -1 or ( self.type_ != 'NOT' and self.input2 == -1 ):
			return

		if self.type_ == 'AND':
			output = 1 if self.input1 == 1 and self.input2 == 1 else 0
		elif self.type_ == 'OR':
			output = 1 if self.input1 == 1 or self.input2 == 1 else 0
		elif self.type_ == 'NOT':
			output = 1 if self.input1 == 0 else 0
		elif self.type_ == 'NAND':
			output = 0 if self.input1 == 1 and self.input2 == 1 else 1
		elif self.type_ == 'NOR':
			output = 0 if self.input1 == 1 or self.input2 == 1 else 1

		for line in self.outputLines:
			line.value = output
		for line in self.outputLines:
			line()
