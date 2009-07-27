TARGET := inject
HOOK   := shell

.PHONY:
all: $(TARGET)

$(TARGET): sfx $(HOOK)
	echo -n '__METAMAGIC__' | cat sfx - $(HOOK) > $@
	chmod a+x $@

sfx: sfx.cpp
	$(CXX) -Os -o $@ $<
	strip $@

.PHONY:
clean:
	rm -f sfx
.PHONY:
distclean: clean
	rm -f $(TARGET)
