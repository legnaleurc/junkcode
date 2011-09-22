.PHONY: all release debug

all: release

release: tile
tile: tile.cpp
	$(CXX) -o $@ -O3 -std=c++0x $^

debug: tiled
tiled: tile.cpp
	$(CXX) -o $@ -g $^

test: release
	@./tile < input.txt
