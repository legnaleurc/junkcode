.PHONY: all clean

all: detect_archive

detect_archive: detect_archive.cpp
	$(CXX) -o $@ -O3 -std=c++11 $$(pkg-config --cflags --libs uchardet) $+

distclean:
	$(RM) detect_archive
