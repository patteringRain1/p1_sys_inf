CFLAGS=-std=c11 -pedantic -Wall -Wvla -Werror -D_DEFAULT_SOURCE -lpthread

PROGS=philosophe

all: $(PROGS)

philosophe: philosophe.o 
	gcc $(CFLAGS) -o philosophe philosophe.o 

philosophe.o: philosophe.c
	gcc $(CFLAGS) -c philosophe.c

clean:
	rm -f *.o
	rm -f $(PROGS)
