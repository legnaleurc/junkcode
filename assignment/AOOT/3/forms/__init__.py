import os, sys, subprocess

__uicPath__ = os.path.join( sys.prefix, u'Scripts', u'pyside-uic' ) if os.name == 'nt' else u'pyside-uic'

for root, dirs, files in os.walk( __path__[0] ):
	for f in files:
		name, ext = os.path.splitext( f )
		if ext != '.ui':
			continue
		uiPath = os.path.join( root, f )
		pyPath = os.path.join( root, '{0}.py'.format( name ) )
		uiMT = os.path.getmtime( uiPath )
		if not ( os.path.exists( pyPath ) and os.path.getmtime( pyPath ) < uiMT ):
			subprocess.call( [ __uicPath__, uiPath, u'-o', pyPath ] )
