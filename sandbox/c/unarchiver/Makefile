all: ua

ua: ua.c
	$(CC) -o $@ -std=c11 -g -O0 `pkg-config --cflags --libs libarchive` $+
