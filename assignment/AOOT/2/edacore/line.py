class Line:
	def __init__( self, name, start, end, port ):
		self.name = name
		if start != None:
			start.outputLines.append( self )
		if end == None:
			# circuit
			self.end = None
			self.port = None
		else:
			self.end = end
			self.port = port
		self.value = -1

	def __call__( self ):
		print 'LINE {0} {1}'.format( self.name, self.value )
		if self.value == -1:
			return

		if self.end != None:
			if self.port == 2:
				self.end.input2 = self.value
			else:
				self.end.input1 = self.value
			self.end()
		else:
			import edacore
			edacore.currentCircuit.result = self.value
