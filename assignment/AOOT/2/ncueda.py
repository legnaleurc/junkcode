#! /usr/bin/env python
#-*- coding: utf-8 -*-

import edacore
import sys

while True:
	try:
		line = raw_input()
	except EOFError:
		sys.exit( 0 )
	try:
		cmd = edacore.parse( line )
	except Exception:
		raise
		sys.exit( 1 )
	try:
		cmd()
	except Exception:
		raise
		sys.exit( 2 )
