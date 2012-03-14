#! /usr/bin/env python
#-*- coding: utf-8 -*-

SIZE = 20000000
table = [ [ True, i ] for i in xrange( SIZE ) ]
table[0][0] = False
table[1][0] = False

for i in xrange( 2, int( SIZE ** 0.5 ) ):
	if table[i][0]:
		j = i << 1
		while j < SIZE:
			if table[j][0]:
				table[j][0] = False
			j = j + i

prime = [ x[1] for x in filter( lambda x: x[0], table ) ]
