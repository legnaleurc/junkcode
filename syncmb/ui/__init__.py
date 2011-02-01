#! /usr/bin/env python
#-*- coding: utf-8 -*-

import os, sys, subprocess

__uicPath__ = os.path.join( sys.prefix, 'Scripts', 'pyside-uic' ) if os.name == 'nt' else 'pyside-uic'

def __walk__():
	for root, dirs, files in os.walk( __path__[0] ):
		for file in files:
			name, ext = os.path.splitext( file )
			if ext != '.ui':
				continue
			uiPath = os.path.join( root, file )
			pyPath = os.path.join( root, name + '.py' )
			uiMT = os.path.getmtime( uiPath )
			try:
				pyMT = os.path.getmtime( pyPath )
				if pyMT < uiMT:
					raise os.error
			except os.error:
				subprocess.call( [ __uicPath__, uiPath, '-o', pyPath ] )

__walk__()

__all__ = []
