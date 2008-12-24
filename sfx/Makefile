TARGET := inject

all: ${TARGET}

${TARGET}: sfx start
	echo -n '__METAMAGIC__' | cat sfx - start > ${TARGET}
	chmod a+x ${TARGET}

sfx: sfx.cpp
	${CXX} -O3 -o sfx sfx.cpp

clean:
	rm -f sfx

distclean: clean
	rm -f ${TARGET}
