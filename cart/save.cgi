#! /usr/bin/env python
#-*- coding: utf-8 -*-

import cart, sqlite3, sys, json, cgi

print 'Content-Type: text/plain\n'

link = sqlite3.connect( cart.database )
link.row_factory = sqlite3.Row
c = link.cursor()

form = cgi.FieldStorage( keep_blank_values = True )
args = {}
for key in form.keys():
	if key in [ 'phase', 'volume' ]:
		args[key] = int( form.getfirst( key ) )
	elif key in [ 'title', 'vendor', 'uri', 'date' ]:
		args[key] = unicode( form.getfirst( key ), 'utf-8' )
	else:
		# ignore invalid fields
		pass

if 'title' not in args or len( args['title'] ) == 0:
	print json.dumps( '`title` is empty' )
	sys.exit( 0 )

c.execute( 'SELECT * FROM {0} WHERE `title`=:title'.format( cart.table ), args )

result = c.fetchone()
if result is None:
	# new item, insert
	c.execute( 'INSERT INTO %s (title,vendor,uri,date,phase,volume) VALUES(:title,:vendor,:uri,:date,:phase,:volume)' % cart.table, args )
else:
	# item exists, update
	tmp = []
	for key in [ 'vendor', 'uri', 'date', 'phase' ]:
		if key in args:
			tmp.append( '{0}=:{0}'.format( key ) )
	c.execute( 'UPDATE %s SET %s WHERE title=:title' % ( cart.table, ','.join( tmp ) ), args )

link.commit()
c.close()
