TARGET := demo
HEADERS := $(shell find . -name '*.hpp')
SOURCES := $(shell find . -name '*.cpp')
OBJECTS := $(subst .cpp,.o,$(SOURCES))

CXXFLAGS := -O3

RM := rm -rf

.PHONY:
all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o $@
	strip $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

.PHONY:
clean:
	$(RM) *.o *~

.PHONY:
distclean: clean
	$(RM) $(TARGET)

.PHONY:
dist:
	mkdir 494410006 && cp $(SOURCES) $(HEADERS) Makefile 494410006
	-cp report.pdf 494410006
	tar -cjf 494410006.tbz2 494410006
	$(RM) 494410006
