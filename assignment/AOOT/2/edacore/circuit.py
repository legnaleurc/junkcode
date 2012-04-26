class Circuit:
	def __init__( self, name ):
		self.name = name
		self.gates = {}
		self.lines = {}
		self.inputGates = {}
		self.result = -1
		self.lastLine = None
		print 'Welcome to NCUEDA 1.0 we are waiting for your command.'

	def addGate( self, gate ):
		self.gates[gate.name] = gate

	def addLine( self, line ):
		self.lines[line.name] = line
		if line.end == None:
			self.lastLine = line

	def evaluate( self, data ):
		for i in data:
			self.lines[i[0]].value = i[1]
			self.lines[i[0]]()
		print 'Circuit {0} output are:'.format( self.name )
		print '{0} = {1}'.format( self.lastLine.name, self.result )

	def getGate( self, name ):
		if name == 'input':
			return None
		elif name == 'output':
			return None
		else:
			return self.gates[name]

	def listAll( self ):
		print 'Gate information:'
		for n, g in self.gates.iteritems():
			print g
		print 'Line information:'
		for n, l in self.lines.iteritems():
			print l
