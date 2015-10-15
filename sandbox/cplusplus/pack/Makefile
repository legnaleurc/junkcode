all: release

release: pack

pack: main.cpp pack.a
	$(CXX) -o $@ -DNDEBUG -std=c++0x -O3 $+

pack.a: pack.cpp pack.hpp
	$(CXX) -o $@ -std=c++0x -c $<
