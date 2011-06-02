#! /usr/bin/env python
#-*- coding: utf-8 -*-

import cart, sqlite3, json

link = sqlite3.connect( cart.database )
link.row_factory = sqlite3.Row
c = link.cursor()

c.execute( 'SELECT * FROM %s ORDER BY date, title' % cart.table )

result = []
for row in c:
	tmp = {}
	for key in row.keys():
		tmp[key] = row[key]
	result.append( tmp )

c.close()
link.close()

print 'Content-Type: text/plain; charset="utf-8"'
print
print json.dumps( result )
