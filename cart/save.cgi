#! /usr/bin/env python
#-*- coding: utf-8 -*-

import cart, sqlite3, sys, json, cgi

print 'Content-Type: text/html'
print

link = sqlite3.connect( cart.database )
link.row_factory = sqlite3.Row
c = link.cursor()

form = cgi.FieldStorage( keep_blank_values = True )
args = {}
for key in form.keys():
	args[key] = form.getfirst( key )
if 'title' not in args or len( args['title'] ) == 0:
	print json.dumps( '`title` is empty' )
	sys.exit( 0 )

c.execute( 'SELECT * FROM %s WHERE `title`=:title' % cart.table, args )

result = c.fetchone()
if result is None:
	# new item, insert
	c.execute( 'INSERT INTO %s (title,vendor,uri,date,done,volume) VALUES(:title,:vendor,:uri,:date,:done,:volume)' % cart.table, args )
else:
	# item exists, update
	tmp = []
	for key in [ 'uri', 'date', 'done' ]:
		if key in form:
			tmp.append( '%s=:%s' % ( key, key ) )
	c.execute( 'UPDATE %s SET %s WHERE title=:title' % ( cart.table, ','.join( tmp ) ), args )

link.commit()
c.close()
