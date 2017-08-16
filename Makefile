DESTDIR=/home/al/bin
wordscapes: wordscapes.c
	$(CC) $^ -o$@

install: wordscapes
	mkdir -p $(DESTDIR)
	cp wordscapes $(DESTDIR)/wordscapes
