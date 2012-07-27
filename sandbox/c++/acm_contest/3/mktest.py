#! /usr/bin/env python
#-*- coding: utf-8 -*-

import random

def g( total, seed ):
	o = []
	for i in xrange( total ):
		tmp = long( 0.5 + random.random() * 2.5 * seed );
		o.append( tmp )
	return o

def h():
	total = random.randint( 8, 16 )
	seed = long( random.random() * 100 )
	o = g( total, seed )
	limit = total * seed
	return limit, o

print 10
for i in xrange( 10 ):
	limit, o = h()
	print '{0} {1}'.format( limit, len( o ) )
	print ' '.join( map( lambda x: str( x ), o ) )
