#! /usr/bin/env python
#-*- coding: utf-8 -*-

import sys, os, re, stat
import cgitb; cgitb.enable()

class Visitor():
	
	def __init__( self, top = None ):
		self.top = '.' if top is None else top
	
	def dirTemplate( self, dirPart, basePart ):
		print '<strong>%s</strong>' % basePart
	
	def fileTemplate( self, dirPart, basePart ):
		if self.acceptFile( basePart ):
			pathName = os.path.join( dirPart, basePart )
			fileSize = os.stat( pathName.encode( 'utf8' ) )[stat.ST_SIZE]
			print '<a href="%s" title="%d B">%s</a>' % ( pathName.encode( 'utf8' ), fileSize, basePart.encode( 'utf8' ) )
	
	def elseTemplate( self, dirPart, basePart ):
		print '<span style="color: red;">%s</span>' % os.path.join( dirPart, basePart )
	
	def acceptDir( self, dirName ):
		return re.match( r'^[^\.].+$', dirName ) is not None
	
	def acceptFile( self, fileName ):
		return re.match( r'^[^\.].*?[^~]$', fileName ) is not None
	
	def __walk__( self, top ):
		print '<ul>'
		
		for f in os.listdir( top ):
			f = f.decode( 'utf8' )
			pathName = os.path.join( top, f )
			mode = os.stat( pathName.encode( 'utf8' ) )[stat.ST_MODE]
			if stat.S_ISDIR( mode ) and self.acceptDir( f ):
				self.dirTemplate( top, f )
				self.__walk__( pathName )
			elif stat.S_ISREG( mode ) and self.acceptFile( f ):
				print '<li>'
				self.fileTemplate( top, f )
				print '</li>'
			else:
				print '<li>'
				self.elseTemplate( top, f )
				print '</li>'
		
		print '</ul>'
	
	def walk( self ):
		self.__walk__( self.top )

def main( args = None ):
	if args is None:
		args = sys.argv
	
	print 'Content-type: text/html\n'
	
	print '<?xml version="1.0" encoding="UTF-8"?>'
	print '<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">'
	print '<html xmlns="http://www.w3.org/1999/xhtml">'
	print '<head>'
	print '<title>Material</title>'
	print '</head>'
	print '<body>'
	v = Visitor( unicode( '.' ) )
	v.walk()
	print '</body>'
	print '</html>'
	
	return 0

if __name__ == '__main__':
	sys.exit( main() )
