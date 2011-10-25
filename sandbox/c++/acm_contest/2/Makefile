.PHONY: all release debug test

all: release

release: join

join: join.cpp
	$(CXX) -std=c++0x -O3 $+ -o $@

debug: joind

joind: join.cpp
	$(CXX) -std=c++0x -g $+ -o $@

test: join
	./join < input.txt
