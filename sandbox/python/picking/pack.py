import random, math, copy

class GeneticAlgorithm:

	class Cell:
		def __init__( self, table, value ):
			self.table = table
			self.value = value

		def __cmp__( self, other ):
			return cmp( other.value, self.value )

		def __repr__( self ):
			return '({0},{1})'.format( self.value, self.table )

		def canToggle( self, key, value, limit ):
			return ( self.table[key] or self.value + value < limit )

		def toggle( self, key, value ):
			tmp = self.table[key]
			self.table[key] = not tmp
			if tmp:
				self.value = self.value - value
			else:
				self.value = self.value + value

	def __init__( self, limit, items ):
		self.limit = limit
		self.table = items

		self.population = sorted( [ self.generatePopulation() for n in xrange( len( items ) ) ] )

	def __call__( self ):
		while not self.canStop():
			print self.population
			self.crossOver()
			self.mutation()
			self.population.sort()
			self.population = self.population[:len( self.table )]

		survivor = self.population[0]
		result = ( survivor.value, filter( lambda k: survivor.table[k], survivor.table.iterkeys() ) )
		return result

	def canStop( self ):
		head = self.population[0]
		tail = self.population[-1]
		return head == tail

	def crossOver( self ):
		length = len( self.population )
		for i in xrange( length ):
			new1 = copy.deepcopy( self.population[i] )
			new2 = copy.deepcopy( self.population[ self.selectParent() ] )
			for k, v in self.table.iteritems():
				if new1.table[k] == new2.table[k]:
					continue
				if not new1.canToggle( k, v, self.limit ) or not new2.canToggle( k, v, self.limit ):
					continue
				if random.random() < 0.5:
					new1.toggle( k, v )
					new2.toggle( k, v )
			self.population.append( new1 )
			self.population.append( new2 )

	def generatePopulation( self ):
		cell = {}
		sum_ = 0L
		for k, v in self.table.iteritems():
			if v + sum_ >= self.limit or random.random() * 2 < 1.0:
				cell[k] = False
			else:
				cell[k] = True
				sum_ = sum_ + v
		return GeneticAlgorithm.Cell( cell, sum_ )

	def mutation( self ):
		length = len( self.population )
		for i in xrange( length ):
			cell = self.population[i]
			for k, v in self.table.iteritems():
				if cell.canToggle( k, v, self.limit ) and random.random() * len( self.table ) < 1.0:
					cell.toggle( k, v )

	def selectParent( self ):
		n = len( self.table )
		b = 2 * n - 1
		c = random.random()
		d = ( b * b - 1 ) * ( 1 - c ) + 1
		k = ( b - d ** 0.5 ) / 2
		return int( math.floor( k ) )

def geneticAlgorithm( limit, items ):
	return GeneticAlgorithm( limit, items )()

def pick( limit, items ):
	sum_ = 0L
	for v in items.itervalues():
		sum_ = sum_ + v
	if sum_ <= limit:
		return ( limit, items.keys() )
	return geneticAlgorithm( limit, items )

print pick( 10L, { 'a': 5L, 'b': 4L, 'c': 3L, 'd': 2L, 'e': 1L } )
