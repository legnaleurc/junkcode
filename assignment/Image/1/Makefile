TARGET := demo

all: $(TARGET)

$(TARGET): main.o image.o bitmap.o
	$(CXX) $^ -o $@

%.o: %.cpp
	$(CXX) -g -o $@ -c $<

.PHONY:
clean:
	rm *.o
