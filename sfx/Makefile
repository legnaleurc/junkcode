TARGET := inject

all: ${TARGET}

${TARGET}: sfx start
	echo -n '__METAMAGIC__' | cat sfx - start > ${TARGET}
	chmod a+x ${TARGET}

sfx: sfx.cpp
	${CXX} -Os -o $@ $<
	strip $@

clean:
	rm -f sfx

distclean: clean
	rm -f ${TARGET}
