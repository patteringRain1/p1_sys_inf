/*
 * Programme de mesure des performances d’un verrou par attente active
 * (spinlock) implémenté à l’aide d’une opération atomique (xchg).
 *
 * Fonctionnement :
 *   - N threads exécutent chacun un nombre fixe de sections critiques.
 *   - TOTAL_SECTIONS_CRITIQUES est réparti équitablement entre les threads.
 *   - Chaque entrée en section critique est protégée par un spinlock :
 *         lock()   → acquisition active (TAS ou TTAS selon l’implémentation)
 *         unlock() → libération du verrou
 *
 * Objectif du programme :
 *   - Observer l’impact de la contention sur un verrou par attente active.
 *   - Mesurer le temps d’exécution pour différents nombres de threads.
 *
 * Intérêt pédagogique :
 *   - Montrer l’impact des invalidations de cache lors du spinlock.
 *   - Comparer les variantes test-and-set et test-and-test-and-set.
 */


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "spinlock.h"

#define TOTAL_SECTIONS_CRITIQUES 32768  // Nombre total de sections critiques

spinlock_t spinlock;     // Verrou par attente active
volatile int cpt = 0;    // Compteur global, incrémenté dans la SC
int N;                   // Nombre de threads


void* section_critique(void* arg) {
    int items = TOTAL_SECTIONS_CRITIQUES / N;  // Travail par thread

    // Simulation d’un temps de calcul préalable
    for (int j = 0; j < 10000; j++);

    for (int i = 0; i < items; i++) {
        lock(&spinlock);      // Attente active → acquisition du verrou

        /* --- Section critique --- */
        cpt++;
        /* --- Fin section critique --- */

        unlock(&spinlock);    // Libération du verrou
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    N = atoi(argv[1]);  // Récupère le nombre de threads en argument
    int err;

    pthread_t threads[N];

    // Initialisation du spinlock (flag = 0)
    spinlock_init(&spinlock);

    // Création des threads
    for (int i = 0; i < N; i++) {
        err = pthread_create(&threads[i], NULL, section_critique, NULL);
        if (err != 0)
            perror("pthread_create");
    }

    // Attente de terminaison
    for (int i = 0; i < N; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}

