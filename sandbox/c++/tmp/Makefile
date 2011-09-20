.PHONY: all release debug

all: release

release: tile
tile: tile.cpp
	$(CXX) -o $@ -O3 $^

debug: tiled
tiled: tile.cpp
	$(CXX) -o $@ -g $^

test: release
	./tile < input.txt
