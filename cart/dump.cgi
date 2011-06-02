#! /usr/bin/env python
#-*- coding: utf-8 -*-

import cart, sqlite3

link = sqlite3.connect( cart.database )

print 'Content-Disposition: attachment; filename="cart.sql"'
print 'Content-Type: text/plain'
print 'Cache-Control: private, no-cache'
print

for line in link.iterdump():
	print line.encode( 'UTF-8' )
link.close()
