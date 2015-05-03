CC=gcc
LFLAGS=
CFLAGS=-ggdb -O0 -m32 -Wall

arptest: arp.c secret.c utils.c 
	$(CC) $(CFLAGS) $(LFLAGS) $^ -o arptest

clean:
	rm -f arptest
	rm -fr arptest.dSYM/
