TARGET := demo

all: $(TARGET)

$(TARGET): main.o image.o bitmap.o
	$(CXX) -O3 $^ -o $@
	strip $@

%.o: %.cpp
	$(CXX) -O3 -o $@ -c $<

.PHONY:
clean:
	rm *.o *~
