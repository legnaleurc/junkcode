class Circuit:
	def __init__( self, name ):
		self.name = name
		self.gates = {}
		self.lines = {}
		self.inputGates = {}
		self.result = -1

	def addGate( self, gate ):
		self.gates[gate.name] = gate

	def addLine( self, line ):
		self.lines[line.name] = line

	def evaluate( self, data ):
		for i in data:
			self.lines[i[0]].value = i[1]
			self.lines[i[0]]()
		print self.result

	def getGate( self, name ):
		if name == 'input':
			return None
		elif name == 'output':
			return None
		else:
			return self.gates[name]

	def listAll( self ):
		print self.name
		print self.gates
		print self.lines
