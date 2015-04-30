CC=gcc

all: arptest

arptest: arp.c secret.c utils.c 
	$(CC) $(LFLAGS) $^ -o arptest

clean: arptest
	rm -f arptest
