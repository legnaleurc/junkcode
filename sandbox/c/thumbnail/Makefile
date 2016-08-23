STRIP := strip
EXTRA_CFLAGS := -std=c11 -Wall -Werror $$(pkg-config --cflags libavutil libavcodec libavformat libswscale)
EXTRA_LDFLAGS := $$(pkg-config --libs libavutil libavcodec libavformat libswscale)


all: tn

tn: tn.c
	$(CC) -o $@ -O3 $(EXTRA_CFLAGS) $(EXTRA_LDFLAGS) $+
	$(STRIP) $@

debug: tn.c
	$(CC) -o $@ -g -fsanitize=address $(EXTRA_CFLAGS) $(EXTRA_LDFLAGS) $+

clean:
	$(RM) tn debug

distclean: clean
