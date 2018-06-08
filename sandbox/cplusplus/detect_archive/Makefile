.PHONY: all clean

all: detect_archive

detect_archive: detect_archive.cpp
	#$(CXX) -o $@ -O3 -std=c++11 $$(pkg-config --cflags --libs uchardet) $+
	$(CXX) -o $@ -O3 -std=c++11 -I/tmp/z/include/uchardet -L/tmp/z/lib -luchardet $+

distclean:
	$(RM) detect_archive
