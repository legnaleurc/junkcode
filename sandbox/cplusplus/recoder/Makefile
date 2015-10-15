.PHONY: all clean distclean

all: recorder

recorder: main.o input.o
	$(CXX) $+ -o $@

%.o: %.cpp
	$(CXX) -c $< -o $@

clean:
	@$(RM) -rf *.o

distclean: clean
	@$(RM) recorder
