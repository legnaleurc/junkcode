#! /usr/bin/env python
#-*- coding: utf-8 -*-

import sqlite3, sys, json, cgi, cart

print 'Content-Type: text/plain; charset="utf-8"\n'

link = sqlite3.connect( cart.database )
link.row_factory = sqlite3.Row
c = link.cursor()

form = cgi.FieldStorage()
args = { 'title': form.getfirst( 'title' ).decode( 'UTF-8' ) }
if args['title'] is None:
	print json.dumps( '`title` is empty' )
	sys.exit( 0 )

c.execute( 'SELECT `title` FROM {0} WHERE `title`=:title'.format( cart.table ), args )

result = c.fetchone()
if result is not None:
	c.execute( 'DELETE FROM {0} WHERE `title`=:title'.format( cart.table ), args )
else:
	print json.dumps( '{0} is not found'.format( args['title'] ) )

link.commit()
c.close()
