CFLAGS= -std=c99 -Wall -pedantic -O2

wordscapes: wordscapes.c
	$(CC) $(CFLAGS) $^ -o$@

clean:
	rm wordscapes
