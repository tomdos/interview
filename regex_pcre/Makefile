CC=gcc
LFLAGS=-L/tmp/pcre2-10.10/COMP/lib
CFLAGS=-ggdb -O0 -I/tmp/pcre2-10.10/COMP/include

regex: main.c
	$(CC) $(CFLAGS) $(LFLAGS) $^ -o regex

clean:
	rm -f regex
	rm -fr regex.dSYM/
