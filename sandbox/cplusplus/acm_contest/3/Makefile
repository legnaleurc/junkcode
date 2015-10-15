.PHONY: all release debug test

all: release

release: dfs

debug: dfs_d

dfs: dfs.cpp
	$(CXX) -o $@ -std=c++0x -O3 -DNDEBUG $+

dfs_d: dfs.cpp
	$(CXX) -o $@ -std=c++0x -g $+

test: dfs
	./mktest.py | ./dfs
