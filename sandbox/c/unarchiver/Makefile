all: ua

ua: ua.c
	$(CC) -o $@ -g -O3 `pkg-config --cflags --libs libarchive` $+
