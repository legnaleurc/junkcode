.PHONY: all release clean distclean

all: release

release: pycpp.so

pycpp.so: pycpp.o Base.o Derived.o
	$(CXX) -shared $$(python3-config --libs) -lboost_python-py35 -o $@ $+

pycpp.o: pycpp.cpp
	$(CXX) $$(python3-config --includes) -fPIC -pipe -O3 -c $+ -o $@

Base.o: Base.cpp
	$(CXX) -fPIC -pipe -O3 -c $+ -o $@

Derived.o: Derived.cpp
	$(CXX) -fPIC -pipe -O3 -c $+ -o $@

clean:
	$(RM) *.o

distclean: clean
	$(RM) pycpp.so
