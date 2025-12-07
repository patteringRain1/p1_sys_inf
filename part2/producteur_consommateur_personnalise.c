/*
 * Implémentation du problème Producteurs / Consommateurs utilisant :
 *   - un buffer circulaire de taille fixe (TAILLE),
 *   - un spinlock pour la protection du buffer,
 *   - des sémaphores maison (monsemaphore) pour gérer empty/full.
 *
 * Synchronisation :
 *   - empty : nombre de cases libres dans le buffer,
 *   - full  : nombre de cases remplies,
 *   - mutex : spinlock assurant l'exclusion mutuelle sur le buffer,
 *             position_producteur et position_consommateur.
 *
 * Particularités :
 *   - Les sémaphores monsem_wait/monsem_post reposent sur une boucle
 *     d’attente active (TTAS) et non sur les sémaphores du noyau,
 *     ce qui permet d’étudier les performances du programme en 
 *     environnement entièrement “user-space”.
 *
 * Objectifs :
 *   - Empêcher le buffer d’être sur-rempli (full → attendu par producteurs).
 *   - Empêcher la lecture d’un buffer vide (empty → attendu par consommateurs).
 *   - Garantir l’absence de perte/duplication d’éléments.
 */


#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "spinlock.h"
#include "monsemaphore.h"

#define TAILLE 8
#define ELEM_PROD 131072

volatile int cpt_prod = 0;   // Compteur total d'éléments produits
volatile int cpt_cons = 0;   // Compteur total d'éléments consommés

int buffer[TAILLE];          // Buffer circulaire
int position_consommateur = 0;
int position_producteur = 0;

spinlock_t mutex;            // Protection du buffer (exclusion mutuelle)
monsemaphore_t empty;        // Nombre de cases libres
monsemaphore_t full;         // Nombre de cases pleines

int nb_producteur;
int nb_consommateur;


void* produce(void* arg) {
    int id = *((int*)arg);

    /* Répartition équitable du travail entre les producteurs */
    int base = ELEM_PROD / nb_producteur;
    int reste = ELEM_PROD % nb_producteur;
    int items = base + (id < reste ? 1 : 0);

    // Simulation de travail préalable
    for (int j=0; j<10000; j++);

    for (int i=0; i<items; i++) {
        monsem_wait(&empty);          // Attend une case libre
        lock(&mutex);                 // Début section critique

        buffer[position_producteur] = id;
        position_producteur = (position_producteur + 1) % TAILLE;
        cpt_prod++;

        unlock(&mutex);               // Fin section critique
        monsem_post(&full);           // Signale une case pleine
    }
    return NULL;
}

void* consume(void* arg) {
    int id = *((int*)arg);
    int valeur;

    /* Répartition équitable entre les consommateurs */
    int base  = ELEM_PROD / nb_consommateur;
    int reste = ELEM_PROD % nb_consommateur;
    int items = base + (id < reste ? 1 : 0);

    // Simulation de travail préalable
    for (int j=0; j<10000; j++);

    for (int i=0; i<items; i++) {
        monsem_wait(&full);           // Attend un élément disponible
        lock(&mutex);                 // Début section critique

        valeur = buffer[position_consommateur];
        position_consommateur = (position_consommateur + 1) % TAILLE;

        printf("%d\n", valeur);
        cpt_cons++;

        unlock(&mutex);               // Fin section critique
        monsem_post(&empty);          // Signale une case libérée
    }
    return NULL;
}


int main(int argc, char const *argv[])
{
    nb_producteur   = atoi(argv[1]);
    nb_consommateur = atoi(argv[2]);

    pthread_t *thread_prod = malloc(nb_producteur * sizeof(pthread_t));
    pthread_t *thread_cons = malloc(nb_consommateur * sizeof(pthread_t));

    /* Initialisation des primitives de synchronisation */
    spinlock_init(&mutex);
    monsem_init(&empty, TAILLE);   // TAILLE cases libres au début
    monsem_init(&full, 0);         // 0 cases pleines

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

    /* Attente de fin des threads */
    for (int i = 0; i < nb_producteur; i++)
        pthread_join(thread_prod[i], NULL);

    for (int i = 0; i < nb_consommateur; i++)
        pthread_join(thread_cons[i], NULL);

    printf("valeur du cpt_prod : %d\n", cpt_prod);
    printf("valeur du cpt_cons : %d\n", cpt_cons);

    return 0;
}

