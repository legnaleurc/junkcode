TARGET := quine
SELF   := quine.cpp

.PHONY:
all: $(TARGET)

$(TARGET): $(SELF)
	$(CXX) -std=c++11 -Os -o $@ $<
	strip $@
	echo -n '__METAMAGIC__' | cat - $(SELF) >> $@

.PHONY:
clean:

.PHONY:
distclean: clean
	rm -f $(TARGET)
