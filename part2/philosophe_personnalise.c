/*
 * Implémentation du problème des philosophes (Dining Philosophers Problem)
 * utilisant un spinlock (verrou par attente active) comme mécanisme
 * d'exclusion mutuelle pour représenter chaque baguette.
 *
 * Stratégie :
 *   - Chaque philosophe tente de prendre toujours la baguette de plus
 *     petit index en premier. Cet ordre global d'acquisition permet
 *     d'éviter les interblocages (deadlocks).
 *
 * Particularité :
 *   - Le spinlock peut être implémenté en test-and-set (TAS) ou
 *     test-and-test-and-set (TTAS), ce qui permet de mesurer leur
 *     impact sur la contention et les performances.
 *
 * Fonctionnement :
 *   - Chaque philosophe répète NB_CYCLES cycles consistant à acquérir
 *     ses deux baguettes, exécuter une section critique (mange), puis
 *     relâcher les verrous.
 */


#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "spinlock.h"

#define NB_CYCLES 1000000

// Variables globales
spinlock_t *baguette;     // Tableau de verrous représentant les baguettes
int nb_philosophes;       // Nombre de philosophes (et donc de baguettes)


void mange(int id)
{
    // Fonction vide : la mesure se fait sur les verrous uniquement
}


void *philosophe(void *arg)
{
    int id = *(int *)arg;
    int left = id;                              // Baguette gauche
    int right = (id + 1) % nb_philosophes;      // Baguette droite (circulaire)

    for (long cycle = 0; cycle < NB_CYCLES; cycle++)
    {
        /*
         * Pour éviter les deadlocks :
         * on impose un ordre global d'acquisition.
         * Le philosophe verrouille toujours la baguette à index le plus petit
         * avant celle de plus grand index.
         */
        if (left < right) {
            lock(&baguette[left]);
            lock(&baguette[right]);
        } else {
            lock(&baguette[right]);
            lock(&baguette[left]);
        }

        mange(id);         // --- Section critique ---

        // Libération des baguettes
        unlock(&baguette[left]);
        unlock(&baguette[right]);
    }

    return NULL;
}


int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s N_philosophes\n", argv[0]);
        return EXIT_FAILURE;
    }

    nb_philosophes = atoi(argv[1]);
    if (nb_philosophes <= 1) {
        fprintf(stderr, "N_philosophes doit être > 1\n");
        return EXIT_FAILURE;
    }

    // Allocation du tableau de spinlocks (un par baguette)
    baguette = malloc(sizeof(spinlock_t) * nb_philosophes);
    if (!baguette) {
        perror("malloc");
        return EXIT_FAILURE;
    }

    // Initialisation des spinlocks
    for (int i = 0; i < nb_philosophes; ++i)
        spinlock_init(&baguette[i]);

    // Allocation des threads et de leurs IDs
    pthread_t *threads = malloc(sizeof(pthread_t) * nb_philosophes);
    int *ids = malloc(sizeof(int) * nb_philosophes);

    if (!threads || !ids) {
        perror("malloc");
        return EXIT_FAILURE;
    }

    // Création des threads philosophes
    for (int i = 0; i < nb_philosophes; ++i) {
        ids[i] = i;
        if (pthread_create(&threads[i], NULL, philosophe, &ids[i]) != 0) {
            perror("pthread_create");
            return EXIT_FAILURE;
        }
    }

    // Attente de fin d'exécution
    for (int i = 0; i < nb_philosophes; ++i)
        pthread_join(threads[i], NULL);

    free(baguette);
    free(threads);
    free(ids);

    return EXIT_SUCCESS;
}

