.PHONY: all release debug profile test

all: release

release: tile
tile: tile.cpp
	$(CXX) -o $@ -O3 -std=c++0x $^

debug: tiled
tiled: tile.cpp
	$(CXX) -o $@ -g -std=c++0x $^

profile: tilep
tilep: tile.cpp
	$(CXX) -o $@ -O3 -pg -std=c++0x $^

test: release
	@$(PWD)/tile < input.txt
