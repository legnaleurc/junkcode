.PHONY: all release debug

all: release

release: join

join: join.cpp
	$(CXX) -std=c++0x -O3 $+ -o $@

debug: joind

joind: join.cpp
	$(CXX) -std=c++0x -g $+ -o $@
