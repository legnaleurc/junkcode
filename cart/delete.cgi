#! /usr/bin/env python
#-*- coding: utf-8 -*-

import sqlite3, sys, json, cgi, cart

print 'Content-Type: text/plain; charset="utf-8"\n'

link = sqlite3.connect( cart.database )
link.row_factory = sqlite3.Row
c = link.cursor()

form = cgi.FieldStorage()
title = form.getfirst( 'title', '' ).decode( 'UTF-8' )
if len( title ) == 0:
	print json.dumps( '`title` is empty' )
	sys.exit( 0 )

c.execute( 'SELECT `title` FROM {0} WHERE `title`=?'.format( cart.table ), ( title, ) )

result = c.fetchone()
if result is not None:
	c.execute( 'DELETE FROM {0} WHERE `title`=?'.format( cart.table ), ( title, ) )
	link.commit()
else:
	print json.dumps( '{0} is not found'.format( title ) )

c.close()
link.close()
