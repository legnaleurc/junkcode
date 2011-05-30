#! /usr/bin/env python
#-*- coding: utf-8 -*-

import cgitb; cgitb.enable()
import sites, json, cgi

form = cgi.FieldStorage()
uri = form.getfirst( 'uri' )
data = sites.fetch( uri )

print 'Content-Type: text/html\n'

print json.dumps( data )
