CFLAGS=-std=c11 -pedantic -Wall -Werror -D_DEFAULT_SOURCE -lpthread

PROGS=philosophe lecteurs_ecrivains producteurs_consommateurs test_and_set test_and_test_and_set

all: $(PROGS)

# Partie 1

# 1.1. Philosophes

philosophe: philosophe.o
	gcc $(CFLAGS) -o philosophe philosophe.o

philosophe.o: philosophe.c
	gcc $(CFLAGS) -c philosophe.c

# 1.2. Lecteurs et écrivains

lecteurs_ecrivains: lecteurs_ecrivains.o
	gcc $(CFLAGS) -o lecteurs_ecrivains lecteurs_ecrivains.o

lecteurs_ecrivains.o: lecteurs_ecrivains.c
	gcc $(CFLAGS) -c lecteurs_ecrivains.c

# 1.3. Producteurs et consommateurs

producteurs_consommateurs: producteurs_consommateurs.o
	gcc $(CFLAGS) -o producteurs_consommateurs producteurs_consommateurs.o

producteurs_consommateurs.o: producteurs_consommateurs.c
	gcc $(CFLAGS) -c producteurs_consommateurs.c

# Partie 2

main.o: main.c
	gcc $(CFLAGS) -c main.c

# 2.2. Test and Set

test_and_set: main.o test_and_set.o
	gcc $(CFLAGS) $(LDFLAGS) -o test_and_set main.o test_and_set.o

test_and_set.o: test_and_set.c
	gcc $(CFLAGS) -c test_and_set.c 

# 2.3. Test and Test and Set

test_and_test_and_set: main.o test_and_test_and_set.o
	gcc $(CFLAGS) $(LDFLAGS) -o test_and_test_and_set main.o test_and_test_and_set.o

test_and_test_and_set.o: test_and_test_and_set.c
	gcc $(CFLAGS) -c test_and_test_and_set.c

clean:
	rm -f *.o
	rm -f $(PROGS)
