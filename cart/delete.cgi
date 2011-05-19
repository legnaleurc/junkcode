#! /usr/bin/env python
#-*- coding: utf-8 -*-

import sqlite3, sys, json, cgi, cart

print 'Content-Type: text/html'
print

link = sqlite3.connect( cart.database )
link.row_factory = sqlite3.Row
c = link.cursor()

form = cgi.FieldStorage()
args = {}
for key in form.keys():
	args[key] = form.getfirst( key )
if 'title' not in args or len( args['title'] ) == 0:
	print json.dumps( '`title` is empty' )
	sys.exit( 0 )

c.execute( 'SELECT `title` FROM %s WHERE `title`=:title' % cart.table, args )

result = c.fetchone()
if result is not None:
	c.execute( 'DELETE FROM %s WHERE `title`=:title' % cart.table, args )

link.commit()
c.close()
