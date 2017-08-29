DESTDIR=/home/al/bin
CFLAGS   = -std=c99 -Wall -pedantic -O2
CC=gcc

wordscapes: wordscapes.c
	$(CC) $(CFLAGS) $^ -o$@

install: wordscapes
	mkdir -p $(DESTDIR)
	cp wordscapes $(DESTDIR)/wordscapes
