.PHONY: all release debug profile test

all: release

release: join

join: join.cpp
	$(CXX) -std=c++0x -O3 $+ -o $@

debug: joind

joind: join.cpp
	$(CXX) -std=c++0x -g $+ -o $@

profile: joinp

joinp: join.cpp
	$(CXX) -std=c++0x -O3 -pg $+ -o $@

test: join
	@./join < input.txt
