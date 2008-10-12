#! /usr/bin/env python
#-*- coding: utf-8 -*-

import sys, os, re, stat
import cgitb; cgitb.enable()

class Visitor():
	
	def __init__( self, top = None ):
		self.top = '.' if top is None else top
		self.dirTemplate = '<strong>%s</strong>'
		self.fileTemplate = '<a href="%s">%s</a>'
		self.elseTemplate = '<span style="color: red;">%s</span>'
	
	def __walk__( self, top ):
		print '<ul>'
		
		for f in os.listdir( top ):
			print '<li>'
			
			pathName = os.path.join( top, f )
			mode = os.stat( pathName )[stat.ST_MODE]
			if stat.S_ISDIR( mode ):
				print self.dirTemplate % f
				self.__walk__( pathName )
			elif stat.S_ISREG( mode ):
				print self.fileTemplate % ( pathName, f )
			else:
				print self.elseTemplate % pathName
			
			print '</li>'
						
		print '</ul>'
	
	def walk( self ):
		self.__walk__( self.top )

def main( args = None ):
	if args is None:
		args = sys.argv
	
	print 'Content-type: text/html\n'
	
	v = Visitor( '../Document' )
	v.walk()
	
	return 0

if __name__ == '__main__':
	sys.exit( main() )
