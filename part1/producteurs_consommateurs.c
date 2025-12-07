/*
 * Implémentation du problème Producteurs / Consommateurs
 * utilisant un buffer circulaire de taille fixe (TAILLE).
 *
 * Synchronisation :
 *   - mutex : protège l’accès au buffer et aux indices de lecture/écriture.
 *   - empty : sémaphore comptant le nombre de cases libres.
 *   - full  : sémaphore comptant le nombre de cases remplies.
 *
 * Fonctionnement :
 *   - Chaque producteur insère un nombre fixé d’éléments ELEM_PROD
 *     dans le buffer, répartis équitablement entre les threads.
 *   - Chaque consommateur retire également un nombre égal d’éléments.
 *   - Le buffer est partagé selon un protocole classique :
 *        * attendre empty → entrée dans section critique → produire → signal full
 *        * attendre full  → entrée dans section critique → consommer → signal empty
 *
 * Objectifs :
 *   - Garantir l’absence de perte ou duplication de données.
 *   - Empêcher les dépassements du buffer (plein / vide).
 */


#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>
#include <stdbool.h>

#define TAILLE 8          // Taille du buffer circulaire
#define ELEM_PROD 131072  // Nombre total d'éléments à produire

volatile int cpt_prod = 0;   // Compteur total de productions
volatile int cpt_cons = 0;   // Compteur total de consommations

int buffer[TAILLE];
int position_consommateur = 0;
int position_producteur = 0;

pthread_mutex_t mutex;        // Mutex pour protéger le buffer
sem_t empty, full;            // Sémaphores empty / full

int nb_producteur;
int nb_consommateur;



void* produce(void* arg) {
    int id = *((int*)arg);

    /* Répartition équitable du travail entre producteurs */
    int base = ELEM_PROD / nb_producteur;
    int reste = ELEM_PROD % nb_producteur;
    int items = base + (id < reste ? 1 : 0);

    // Simulation d’un coût préalable
    for (int j=0; j<10000; j++);

    for (int i=0; i<items; i++) {
        sem_wait(&empty);              // Attend une case libre
        pthread_mutex_lock(&mutex);    // Début section critique

        buffer[position_producteur] = id;
        position_producteur = (position_producteur + 1) % TAILLE;
        cpt_prod++;

        pthread_mutex_unlock(&mutex);  // Fin section critique
        sem_post(&full);               // Signale une case pleine
    }
    return NULL;
}


void* consume(void* arg) {
    int id = *((int*)arg);
    int valeur;

    /* Répartition équitable du travail entre consommateurs */
    int base  = ELEM_PROD / nb_consommateur;
    int reste = ELEM_PROD % nb_consommateur;
    int items = base + (id < reste ? 1 : 0);

    // Simulation d’un coût préalable
    for (int j=0; j<10000; j++);

    for (int i=0; i<items; i++) {
        sem_wait(&full);               // Attend une case pleine
        pthread_mutex_lock(&mutex);    // Début section critique

        valeur = buffer[position_consommateur];
        position_consommateur = (position_consommateur + 1) % TAILLE;

        printf("%d\n", valeur);        // Lecture de l'élément
        cpt_cons++;

        pthread_mutex_unlock(&mutex);  // Fin section critique
        sem_post(&empty);              // Signale une case libre
    }
    return NULL;
}

int main(int argc, char const *argv[])
{
    nb_producteur   = atoi(argv[1]);
    nb_consommateur = atoi(argv[2]);

    /* Allocation des threads */
    pthread_t *thread_prod = malloc(nb_producteur * sizeof(pthread_t));
    pthread_t *thread_cons = malloc(nb_consommateur * sizeof(pthread_t));

    /* Initialisation des sémaphores */
    pthread_mutex_init(&mutex, NULL);
    sem_init(&empty, 0, TAILLE);   // TAILLE cases libres au départ
    sem_init(&full,  0, 0);        // 0 cases pleines

    /* Création des threads producteurs */
    int *ids_prod = malloc(nb_producteur * sizeof(int));
    for (int i = 0; i < nb_producteur; i++) {
        ids_prod[i] = i;
        pthread_create(&thread_prod[i], NULL, produce, &ids_prod[i]);
    }

    /* Création des threads consommateurs */
    int *ids_cons = malloc(nb_consommateur * sizeof(int));
    for (int i = 0; i < nb_consommateur; i++) {
        ids_cons[i] = i;
        pthread_create(&thread_cons[i], NULL, consume, &ids_cons[i]);
    }

    /* Attente des threads */
    for (int i = 0; i < nb_producteur; i++)
        pthread_join(thread_prod[i], NULL);

    for (int i = 0; i < nb_consommateur; i++)
        pthread_join(thread_cons[i], NULL);

    pthread_mutex_destroy(&mutex);
    sem_destroy(&empty);
    sem_destroy(&full);

    printf("valeur du cpt_prod : %d\n", cpt_prod);
    printf("valeur du cpt_cons : %d\n", cpt_cons);

    return 0;
}

