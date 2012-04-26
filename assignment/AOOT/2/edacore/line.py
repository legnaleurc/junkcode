class Line:
	def __init__( self, name, start, end, port ):
		self.name = name
		if start != None:
			start.outputLines.append( self )
		self.start = start
		if end == None:
			# circuit
			self.end = None
			self.port = None
		else:
			self.end = end
			self.port = port
		self.value = -1
		print 'You have add a line called "{0}" and its two ends are connected:'.format( self.name )
		if start == None:
			print '{0}.START: as input'.format( self.name )
		else:
			print '{0}.START: the output 1 of gate "{1}"'.format( self.name, start.name )
		if end != None:
			print '{0}.END: the input {1} of gate "{2}"'.format( self.name, self.port, self.end.name )
		else:
			print '{0}.END: as output'.format( self.name )

	def __str__( self ):
		inputName = 'input' if self.start == None else '{0}.o1'.format( self.start.name )
		outputName = 'output' if self.end == None else '{0}.i{1}'.format( self.end.name, self.port )
		return 'Line {0}: {1} {2}'.format( self.name, inputName, outputName )

	def __call__( self ):
#		print 'LINE {0} {1}'.format( self.name, self.value )
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
