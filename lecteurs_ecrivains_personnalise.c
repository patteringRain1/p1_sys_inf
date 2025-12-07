/*
 * Implémentation du problème des lecteurs / écrivains utilisant
 * un système de sémaphores maison (monsemaphore), basé sur un
 * verrou par attente active (spinlock).
 *
 * Sémaphores utilisés :
 *   - mutex : protège les accès à readcount et aux compteurs réels.
 *   - wrt   : garantit l'exclusion mutuelle pour les écrivains.
 *   - readtry : bloque l’entrée de nouveaux lecteurs lorsqu’un écrivain attend.
 *
 * Fonctionnement :
 *   - Les écrivains effectuent NOMBRE_ECRITURE écritures réparties
 *     équitablement entre les threads écrivains.
 *   - Les lecteurs effectuent NOMBRE_LECTURE lectures réparties
 *     équitablement entre les threads lecteurs.
 *   - Les sémaphores implémentent un protocole d'accès identique
 *     à celui vu au cours, mais basé sur une attente active plutôt
 *     que sur des primitives du noyau (sem_wait / sem_post).
 *
 * Objectif :
 *   - Préserver la priorité des écrivains (protocole writers-preference).
 *   - Permettre la lecture concurrente entre plusieurs lecteurs.
 */


#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "monsemaphore.h"

#define NOMBRE_ECRITURE 6400
#define NOMBRE_LECTURE 25400

monsemaphore_t mutex;
monsemaphore_t wrt;
monsemaphore_t readtry;
volatile int readcount = 0;
volatile int nb_ecriture_reel = 0;
volatile int nb_lecture_reel = 0;
int nb_lecteur;
int nb_ecrivain;


void* writer(void* arg) {
    /* Calcul du nombre d'écritures attribué à ce thread */
    int id = *((int*) arg);
    int base = NOMBRE_ECRITURE / nb_ecrivain;
    int reste = NOMBRE_ECRITURE % nb_ecrivain;
    int items = base + (id < reste ? 1 : 0);

    for (int i = 0; i < items; i++) {
        monsem_wait(&readtry);   // Empêche l'entrée de nouveaux lecteurs
        monsem_wait(&wrt);       // Exclusion mutuelle pour les écrivains

        // --- Section critique écriture ---
        for (int j = 0; j < 10000; j++);
        nb_ecriture_reel++;
        // --- Fin section critique ---

        monsem_post(&wrt);
        monsem_post(&readtry);
    }
    return NULL;
}

void* reader(void* arg) {
    /* Calcul du nombre de lectures attribué à ce thread */
    int id = *((int*) arg);
    int base = NOMBRE_LECTURE / nb_lecteur;
    int reste = NOMBRE_LECTURE % nb_lecteur;
    int items = base + (id < reste ? 1 : 0);

    for (int i = 0; i < items; i++) {

        /* Phase d'entrée en lecture */
        monsem_wait(&readtry);     // Bloqué si un écrivain attend
        monsem_wait(&mutex);       // Accès exclusif à readcount

        readcount++;
        if (readcount == 1)        // Premier lecteur → bloque les écrivains
            monsem_wait(&wrt);

        monsem_post(&mutex);
        monsem_post(&readtry);

        // --- Section critique lecture ---
        for (int j = 0; j < 10000; j++);
        // --- Fin section critique ---

        monsem_wait(&mutex);
        nb_lecture_reel++;
        monsem_post(&mutex);

        /* Phase de sortie */
        monsem_wait(&mutex);
        readcount--;
        if (readcount == 0)        // Dernier lecteur → libère écrivains
            monsem_post(&wrt);
        monsem_post(&mutex);
    }
    return NULL;
}




int main(int argc, char const *argv[]) {
    /* Récupération du nombre de threads écrivains et lecteurs */
    nb_ecrivain = atoi(argv[1]);
    nb_lecteur = atoi(argv[2]);
    int err;

    // Allocation des tableaux de threads
    pthread_t *thread_ecri = malloc(nb_ecrivain * sizeof(pthread_t));
    pthread_t *thread_lect = malloc(nb_lecteur * sizeof(pthread_t));

    // Initialisation des sémaphores maison (valeur initiale = 1)
    monsem_init(&mutex, 1);
    monsem_init(&wrt, 1);
    monsem_init(&readtry, 1);

    /* Création des threads écrivains */
    int *ids_ecri = malloc(nb_ecrivain * sizeof(int));
    for (int i = 0; i < nb_ecrivain; i++) {
        ids_ecri[i] = i;
        err = pthread_create(&thread_ecri[i], NULL, writer, &ids_ecri[i]);
        if (err != 0)
            perror("pthread_create");
    }

    /* Création des threads lecteurs */
    int *ids_lect = malloc(nb_lecteur * sizeof(int));
    for (int i = 0; i < nb_lecteur; i++) {
        ids_lect[i] = i;
        err = pthread_create(&thread_lect[i], NULL, reader, &ids_lect[i]);
        if (err != 0)
            perror("pthread_create");
    }

    // Attente de tous les threads
    for (int i = 0; i < nb_ecrivain; i++)
        pthread_join(thread_ecri[i], NULL);

    for (int i = 0; i < nb_lecteur; i++)
        pthread_join(thread_lect[i], NULL);

    // Libération mémoire
    free(thread_ecri);
    free(thread_lect);
    free(ids_ecri);
    free(ids_lect);

    printf("nombre de lecture : %d\n", nb_lecture_reel);
    printf("nombre de écriture : %d\n", nb_ecriture_reel);

    return 0;
}

