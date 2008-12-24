all: inject

inject: sfx start
	echo -n '__METAMAGIC__' | cat sfx - start > inject
	chmod a+x inject

sfx: sfx.cpp
	g++ -O3 -o sfx sfx.cpp

distclean:
	rm inject
