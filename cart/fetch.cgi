#! /usr/bin/env python
#-*- coding: utf-8 -*-

import sites, json, cgi

form = cgi.FieldStorage()
uri = form.getfirst( 'uri' )
data = sites.fetch( uri )

print 'Content-Type: text/plain\n'

print json.dumps( data )
