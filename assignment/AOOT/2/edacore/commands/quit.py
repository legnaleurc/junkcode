import edacore, sys, re

def quit():
	print 'Bye! Thank you for using NCUEDA 1.0!'
	sys.exit( 0 )

def parse( line ):
	m = re.match( r'^quit$', line )
	if m == None:
		return None
	return quit
