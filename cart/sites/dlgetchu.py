#! /usr/bin/env python
#-*- coding: utf-8 -*-

import re, urllib, urllib2, cookielib

def verify( uri ):
	if uri.netloc == 'dl.getchu.com':
		return 100
	else:
		return 0

def create( uri ):
	query = {}
	query['action'] = 'aa'
	query['aaR18'] = 'true'
	query['returl'] = uri.geturl()

	opener = urllib2.build_opener( urllib2.HTTPCookieProcessor( cookielib.CookieJar() ) )
	link = opener.open( 'http://dl.getchu.com/index.php', urllib.urlencode( query ) )
	data = { 'error': [] }
	key = None
	for line in link:
		try:
			line = line.decode( 'EUC-JP' )
		except UnicodeError as e:
			data['error'].append( [ unicode( e ), line.decode( 'EUC-JP', 'replace' ) ] )
		if key is None:
			if re.search( ur'images/shosai_tl_new\.gif', line ) is not None:
				key = 'title'
			elif re.search( ur'>サークル</td>', line ) is not None:
				key = 'vendor'
			elif re.search( ur'>登録日</td>', line ) is not None:
				key = 'date'
		elif key == 'title':
			m = re.search( ur'<div.+>(.+)</div>', line )
			if m is not None:
				data[key] = m.group( 1 )
				key = None
		elif key == 'vendor':
			m = re.search( ur'<a.+>(.+)</a>', line )
			if m is not None:
				data[key] = m.group( 1 )
				key = None
		elif key == 'date':
			m = re.search( ur'>(.+)</td>', line )
			if m is not None:
				data[key] = m.group( 1 )
				key = None
	link.close()

	return data
