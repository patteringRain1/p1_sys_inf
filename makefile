CFLAGS=-std=c11 -pedantic -Wall -Werror -D_DEFAULT_SOURCE -lpthread

PROGS=philosophe lecteurs_ecrivains producteurs_consommateurs

all: $(PROGS)

philosophe: philosophe.o
	gcc $(CFLAGS) -o philosophe philosophe.o

philosophe.o: philosophe.c
	gcc $(CFLAGS) -c philosophe.c

lecteurs_ecrivains: lecteurs_ecrivains.o
	gcc $(CFLAGS) -o lecteurs_ecrivains lecteurs_ecrivains.o

lecteurs_ecrivains.o: lecteurs_ecrivains.c
	gcc $(CFLAGS) -c lecteurs_ecrivains.c

producteurs_consommateurs: producteurs_consommateurs.o
	gcc $(CFLAGS) -o producteurs_consommateurs producteurs_consommateurs.o

producteurs_consommateurs.o: producteurs_consommateurs.c
	gcc $(CFLAGS) -c producteurs_consommateurs.c

clean:
	rm -f *.o
	rm -f $(PROGS)
