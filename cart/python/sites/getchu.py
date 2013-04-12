#! /usr/bin/env python
#-*- coding: utf-8 -*-

import urlparse, urllib, urllib2, HTMLParser

tr = { 'soft-title': 'title', 'brandsite': 'vendor', 'tooltip-day': 'date' }

class Parser( HTMLParser.HTMLParser ):
	def __init__( self ):
		HTMLParser.HTMLParser.__init__( self )
		self.currentID = None
		self.data = { 'error': [], 'log': [] }

	def handle_starttag( self, tag, attrs ):
		for key, value in attrs:
			if key == 'id' and value in [ 'soft-title', 'brandsite', 'tooltip-day' ]:
				self.currentID = tr[value]
				return
		self.currentID = None

	def handle_endtag( self, tag ):
		self.currentID = None

	def handle_data( self, data ):
		text = data.strip()
		if self.currentID is not None and len( text ) != 0:
			self.data[self.currentID] = text

def verify( uri ):
	if uri.netloc == 'www.getchu.com':
		return 100
	else:
		return 0

def create( uri ):
	query = urlparse.parse_qs( uri.query )
	for key in query:
		query[key] = query[key][0]
	if( 'gc' not in query ):
		query['gc'] = 'gc'
	uri_ = urlparse.urlunsplit( ( uri.scheme, uri.netloc, uri.path, urllib.urlencode( query ), '' ) )

	link = urllib2.urlopen( uri_ )
	parser = Parser()
	for line in link:
		try:
			line = line.decode( 'EUC-JP' )
			parser.feed( line )
		except UnicodeError as e:
			parser.data['error'].append( [ unicode( e ), line.decode( 'EUC-JP', 'replace' ) ] )
		except HTMLParser.HTMLParseError as e:
			parser.data['error'].append( [ unicode( e ), line ] )
	try:
		parser.close()
	except Exception as e:
		parser.data['error'].append( unicode( e ) )
	link.close()

	return parser.data
