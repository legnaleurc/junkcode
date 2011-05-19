#! /usr/bin/env python
#-*- coding: utf-8 -*-

import sqlite3, cgi, cgitb, cart
cgitb.enable()

link = sqlite3.connect( cart.database )
link.row_factory = sqlite3.Row
c = link.cursor()

form = cgi.FieldStorage()
args = {}
for key in form.keys():
	args[key] = form.getfirst( key )
if 'title' in args or len( args['title'] ) == 0:
	pass

c.execute( 'SELECT `title` FROM %s WHERE `title`=:title' % cart.table, args )

result = c.fetchone()
if result is not None:
	c.execute( 'DELETE FROM %s WHERE `title`=:title' % cart.table, args )

link.commit()
c.close()

print 'Content-Type: text/html'
print
