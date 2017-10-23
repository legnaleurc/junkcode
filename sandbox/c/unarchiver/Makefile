all: ua

ua: ua.c
	$(CC) -o $@ -std=c11 -g -O3 `pkg-config --cflags --libs libarchive` $+
