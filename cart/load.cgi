#! /usr/bin/env python
#-*- coding: utf-8 -*-

import sqlite3, json, cart

link = sqlite3.connect( cart.database )
link.row_factory = sqlite3.Row
c = link.cursor()

c.execute( 'SELECT * FROM %s' % cart.table )

result = []
for row in c:
	tmp = {}
	for key in row.keys():
		tmp[key] = row[key]
	result.append( tmp )

link.commit()
c.close()

def helper( l, r ):
	c = cmp( l['date'], r['date'] )
	if c != 0:
		return c
	return cmp( l['title'], r['title'] )
result.sort( helper )

print 'Content-Type: text/html'
print
print json.dumps( result )
