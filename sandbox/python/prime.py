#! /usr/bin/env python
#-*- coding: utf-8 -*-

def mine( n ):
	table = [ [ True, i ] for i in xrange( n ) ]
	table[0][0] = False
	table[1][0] = False

	for i in xrange( 2, int( n ** 0.5 ) ):
		if table[i][0]:
			j = i << 1
			while j < n:
				if table[j][0]:
					table[j][0] = False
				j = j + i

def eras(n):
	siv=range(n+1)
	siv[1]=0
	sqn=int(round(n**0.5))
	for i in range(2,sqn+1):
		if siv[i]!=0:
			siv[2*i:n/i*i+1:i]=[0]*(n/i-1)
	return filter(None,siv)

def primes_up_to(n):
	"""Generates all primes less than n."""
	from itertools import ifilter
	if n <= 2:
		return
	yield 2
	F = [True] * n
	seq1 = xrange(3, int(n**0.5), 2)
	seq2 = xrange(seq1[-1] + 2, n, 2)
	for p in ifilter(F.__getitem__, seq1):
		yield p
		for q in xrange(p * p, n, 2 * p):
			F[q] = False
	for p in ifilter(F.__getitem__, seq2):
		yield p

SIZE = 20000000

eras( SIZE )
#for x in primes_up_to( SIZE ):
#	pass
