#! /usr/bin/env python
#-*- coding: utf-8 -*-

import os, sys, random

if sys.platform == 'win32':
	import msvcrt
	msvcrt.setmode( sys.stdout.fileno(), os.O_BINARY )

files = []
for dirPath, dirNames, fileNames in os.walk( u'imgs' ):
	for fileName in fileNames:
		type_ = os.path.splitext( fileName )[1]
		type_ = type_[1:].upper()
		files.append( ( os.path.join( dirPath, fileName ), type_ ) )

sys.stdout.write( '#DTV:copyright1.0#' )
sys.stdout.write( '{0: 5}'.format( len( files ) ) )
sys.stdout.write( '@@' )

for filePath, type_ in files:
	x = random.randint( 0, 10000 )
	sys.stdout.write( '{0: 5}'.format( x ) )
	y = random.randint( 0, 10000 )
	sys.stdout.write( '{0: 5}'.format( y ) )
	size = os.path.getsize( filePath )
	sys.stdout.write( '{0: 9}'.format( size ) )
	sys.stdout.write( type_ )

for filePath, type_ in files:
	fin = open( filePath, 'rb' )
	sys.stdout.write( fin.read() )
	fin.close()
