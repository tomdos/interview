CC=gcc
LFLAGS=-L/tmp/pcre2-10.10/COMP/lib -lpcre2-8 -lpcre2-posix
CFLAGS=-ggdb -O0 -I/tmp/pcre2-10.10/COMP/include

regex: main.c re.c re.h
	$(CC) $(CFLAGS) $(LFLAGS) $^ -o regex

clean:
	rm -f regex
	rm -fr regex.dSYM/
