CC=gcc
LFLAGS=
CFLAGS=-ggdb -O0 -m32 

all: arptest

arptest: arp.c secret.c utils.c 
	$(CC) $(CFLAGS) $(LFLAGS) $^ -o arptest

clean: arptest
	rm -f arptest
