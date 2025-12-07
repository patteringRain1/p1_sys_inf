/*
 * Implémentation du problème des lecteurs / écrivains
 * utilisant trois sémaphores :
 *     - mutex : protège les accès à readcount et aux compteurs réels.
 *     - wrt   : garantit l'exclusion mutuelle pour les écrivains.
 *     - readtry : empêche les lecteurs d'entrer lorsque qu'un écrivain attend.
 *
 * Fonctionnement :
 *   - Chaque écrivain effectue NOMBRE_ECRITURE écritures au total,
 *     réparties équitablement entre les threads écrivains.
 *   - Chaque lecteur effectue NOMBRE_LECTURE lectures au total,
 *     réparties équitablement entre les threads lecteurs.
 *   - La section critique est simulée par une boucle vide.
 *
 * But :
 *   - Préserver la priorité des écrivains : lorsqu’un écrivain arrive,
 *     il bloque l’entrée de nouveaux lecteurs (readtry).
 *   - Permettre à plusieurs lecteurs de lire en parallèle.
 */


#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#define NOMBRE_ECRITURE 6400
#define NOMBRE_LECTURE 25400

sem_t mutex;
sem_t wrt;
sem_t readtry;
volatile int readcount = 0;
volatile int nb_ecriture_reel = 0;
volatile int nb_lecture_reel = 0;
int nb_lecteur;
int nb_ecrivain;


void* writer(void* arg) {
    /* Répartition équitable du nombre total d'écritures */
    int id = *((int*) arg);
    int base = NOMBRE_ECRITURE / nb_ecrivain;
    int reste = NOMBRE_ECRITURE % nb_ecrivain;

    int items = base + (id < reste ? 1 : 0);

    for (int i = 0; i < items; i++) {
        sem_wait(&readtry);     // Empêche de nouveaux lecteurs d'entrer
        sem_wait(&wrt);         // Exclusion mutuelle des écrivains

        // --- Section critique écriture ---
        for (int j = 0; j < 10000; j++);
        nb_ecriture_reel++;
        // --- Fin section critique ---

        sem_post(&wrt);         // Libère l'accès aux écrivains
        sem_post(&readtry);     // Les lecteurs peuvent à nouveau entrer
    }
    return NULL;
}


void* reader(void* arg) {
    /* Répartition équitable du nombre total de lectures */
    int id = *((int*) arg);
    int base = NOMBRE_LECTURE / nb_lecteur;
    int reste = NOMBRE_LECTURE % nb_lecteur;

    int items = base + (id < reste ? 1 : 0);

    for (int i = 0; i < items; i++) {

        /* Phase d'entrée en lecture */
        sem_wait(&readtry);     // Bloqué si un écrivain attend
        sem_wait(&mutex);       // Accès exclusif à readcount

        readcount++;
        if (readcount == 1)     // Premier lecteur → bloque écrivains
            sem_wait(&wrt);

        sem_post(&mutex);
        sem_post(&readtry);

        // --- Section critique lecture ---
        for (int j = 0; j < 10000; j++);
        // --- Fin section critique lecture ---

        sem_wait(&mutex);
        nb_lecture_reel++;
        sem_post(&mutex);

        /* Phase de sortie */
        sem_wait(&mutex);
        readcount--;
        if (readcount == 0)     // Dernier lecteur → libère écrivains
            sem_post(&wrt);
        sem_post(&mutex);
    }
    return NULL;
}


int main(int argc, char const *argv[]) {
    /* Récupération du nombre d'écrivains et lecteurs */
    nb_ecrivain = atoi(argv[1]);
    nb_lecteur = atoi(argv[2]);

    // Allocation des threads
    pthread_t *thread_ecri = malloc(nb_ecrivain * sizeof(pthread_t));
    pthread_t *thread_lect = malloc(nb_lecteur * sizeof(pthread_t));

    // Initialisation des sémaphores
    sem_init(&mutex, 0, 1);
    sem_init(&wrt, 0, 1);
    sem_init(&readtry, 0, 1);

    /* Création des threads écrivains */
    int *ids_ecri = malloc(nb_ecrivain * sizeof(int));
    for (int i = 0; i < nb_ecrivain; i++) {
        ids_ecri[i] = i;
        pthread_create(&thread_ecri[i], NULL, writer, &ids_ecri[i]);
    }

    /* Création des threads lecteurs */
    int *ids_lect = malloc(nb_lecteur * sizeof(int));
    for (int i = 0; i < nb_lecteur; i++) {
        ids_lect[i] = i;
        pthread_create(&thread_lect[i], NULL, reader, &ids_lect[i]);
    }

    // Attente des threads
    for (int i = 0; i < nb_ecrivain; i++)
        pthread_join(thread_ecri[i], NULL);

    for (int i = 0; i < nb_lecteur; i++)
        pthread_join(thread_lect[i], NULL);

    // Destruction des sémaphores
    sem_destroy(&mutex);
    sem_destroy(&wrt);
    sem_destroy(&readtry);

    free(thread_ecri);
    free(thread_lect);
    free(ids_ecri);
    free(ids_lect);

    return 0;
}

