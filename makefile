CFLAGS=-std=c11 -pedantic -Wall -Werror -D_DEFAULT_SOURCE -lpthread

PROGS=philosophe lecteurs_ecrivains producteurs_consommateurs test_and_set test_and_test_and_set lecteurs_ecrivains_personnalise philosophe_personnalise producteur_consommateur_personnalise


all: $(PROGS)

# Partie 1
# 1.1. Philosophes

part1/philosophe.o: part1/philosophe.c
	gcc $(CFLAGS) -c part1/philosophe.c -o part1/philosophe.o

philosophe: part1/philosophe.o
	gcc $(CFLAGS) -o part1/philosophe part1/philosophe.o

# 1.2. Lecteurs et écrivains

part1/lecteurs_ecrivains.o: part1/lecteurs_ecrivains.c
	gcc $(CFLAGS) -c part1/lecteurs_ecrivains.c -o part1/lecteurs_ecrivains.o

lecteurs_ecrivains: part1/lecteurs_ecrivains.o
	gcc $(CFLAGS) -o part1/lecteurs_ecrivains part1/lecteurs_ecrivains.o

# 1.3. Producteurs et consommateurs

part1/producteurs_consommateurs.o: part1/producteurs_consommateurs.c
	gcc $(CFLAGS) -c part1/producteurs_consommateurs.c -o part1/producteurs_consommateurs.o

producteurs_consommateurs: part1/producteurs_consommateurs.o
	gcc $(CFLAGS) -o part1/producteurs_consommateurs part1/producteurs_consommateurs.o

# Partie 2

part2/main.o: part2/main.c
	gcc $(CFLAGS) -c part2/main.c -o part2/main.o

# 2.2. Test and Set

part2/test_and_set.o: part2/test_and_set.c
	gcc $(CFLAGS) -c part2/test_and_set.c -o part2/test_and_set.o

test_and_set: part2/main.o part2/test_and_set.o
	gcc $(CFLAGS) $(LDFLAGS) -o part2/test_and_set part2/main.o part2/test_and_set.o

# 2.3. Test and Test and Set

part2/test_and_test_and_set.o: part2/test_and_test_and_set.c
	gcc $(CFLAGS) -c part2/test_and_test_and_set.c -o part2/test_and_test_and_set.o

test_and_test_and_set: part2/main.o part2/test_and_test_and_set.o
	gcc $(CFLAGS) $(LDFLAGS) -o part2/test_and_test_and_set part2/main.o part2/test_and_test_and_set.o
	
	
	
	
# 2.5.1 Lecteurs / Écrivains personnalisé

part2/lecteurs_ecrivains_personnalise.o: part2/lecteurs_ecrivains_personnalise.c part2/monsemaphore.h part2/spinlock.h
	gcc $(CFLAGS) -c part2/lecteurs_ecrivains_personnalise.c -o part2/lecteurs_ecrivains_personnalise.o

part2/test_and_test_and_set_personnalise.o: part2/test_and_test_and_set.c part2/spinlock.h
	gcc $(CFLAGS) -c part2/test_and_test_and_set.c -o part2/test_and_test_and_set_personnalise.o

part2/monsemaphore_personnalise.o: part2/monsemaphore.c part2/monsemaphore.h part2/spinlock.h
	gcc $(CFLAGS) -c part2/monsemaphore.c -o part2/monsemaphore_personnalise.o

lecteurs_ecrivains_personnalise: \
        part2/lecteurs_ecrivains_personnalise.o \
        part2/test_and_test_and_set_personnalise.o \
        part2/monsemaphore_personnalise.o
	gcc $(CFLAGS) -o part2/lecteurs_ecrivains_personnalise \
        part2/lecteurs_ecrivains_personnalise.o \
        part2/test_and_test_and_set_personnalise.o \
        part2/monsemaphore_personnalise.o
	
	
	
	
# 2.5.2 Philosophes personnalisé

part2/philosophe_personnalise.o: part2/philosophe_personnalise.c part2/spinlock.h
	gcc $(CFLAGS) -c part2/philosophe_personnalise.c -o part2/philosophe_personnalise.o

part2/test_and_test_and_set_phil.o: part2/test_and_test_and_set.c part2/spinlock.h
	gcc $(CFLAGS) -c part2/test_and_test_and_set.c -o part2/test_and_test_and_set_phil.o

philosophe_personnalise: \
        part2/philosophe_personnalise.o \
        part2/test_and_test_and_set_phil.o
	gcc $(CFLAGS) -o part2/philosophe_personnalise \
        part2/philosophe_personnalise.o \
        part2/test_and_test_and_set_phil.o



# 2.5.3 Producteur / Consommateur personnalisé

part2/producteur_consommateur_personnalise.o: part2/producteur_consommateur_personnalise.c part2/monsemaphore.h part2/spinlock.h
	gcc $(CFLAGS) -c part2/producteur_consommateur_personnalise.c -o part2/producteur_consommateur_personnalise.o

part2/test_and_test_and_set_pc.o: part2/test_and_test_and_set.c part2/spinlock.h
	gcc $(CFLAGS) -c part2/test_and_test_and_set.c -o part2/test_and_test_and_set_pc.o

part2/monsemaphore_pc.o: part2/monsemaphore.c part2/monsemaphore.h part2/spinlock.h
	gcc $(CFLAGS) -c part2/monsemaphore.c -o part2/monsemaphore_pc.o

producteur_consommateur_personnalise: \
        part2/producteur_consommateur_personnalise.o \
        part2/test_and_test_and_set_pc.o \
        part2/monsemaphore_pc.o
	gcc $(CFLAGS) -o part2/producteur_consommateur_personnalise \
        part2/producteur_consommateur_personnalise.o \
        part2/test_and_test_and_set_pc.o \
        part2/monsemaphore_pc.o



clean:
	rm -f ./part1/*.o ./part2/*.o
	rm -f part1/philosophe part1/lecteurs_ecrivains part1/producteurs_consommateurs
	rm -f part2/test_and_set part2/test_and_test_and_set
	rm -f part2/lecteurs_ecrivains_personnalise part2/philosophe_personnalise part2/producteur_consommateur_personnalise

