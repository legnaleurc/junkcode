TARGET1 := demo1
TARGET2 := demo2
HEADERS := $(shell find . -name '*.hpp')
SOURCES := $(shell find . -name '*.cpp')
OBJECTS := image.o bitmap.o

CXXFLAGS := -O3
#CXXFLAGS := -g

RM := rm -rf

.PHONY:
all: $(TARGET1) $(TARGET2)

$(TARGET1): $(OBJECTS) main1.o
	$(CXX) $(CXXFLAGS) $^ -o $@
	strip $@

$(TARGET2): $(OBJECTS) main2.o
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
