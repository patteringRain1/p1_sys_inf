#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "spinlock.h"
#include "monsemaphore.h"

#define TAILLE 8
#define ELEM_PROD 131072

volatile int cpt_prod = 0;
volatile int cpt_cons = 0;
int buffer[TAILLE];
int position_consommateur = 0;
int position_producteur = 0;
spinlock_t mutex;
monsemaphore_t empty;
monsemaphore_t full;
int nb_producteur;
int nb_consommateur;


void* produce(void* arg) {
	int id = *((int*)arg);
	int base = ELEM_PROD / nb_producteur;
	int reste = ELEM_PROD % nb_producteur;

	int items = base;
	if (id < reste) {
		items += 1;
	}

	for (int j=0; j<10000; j++);

	for (int i=0; i<items; i++) {
		monsem_wait(&empty);
		lock(&mutex);
		//section critique

		buffer[position_producteur] = id;
		position_producteur = (position_producteur + 1) % TAILLE;
		cpt_prod = cpt_prod + 1;

		//section critique
		unlock(&mutex);
		monsem_post(&full);
	}
	return NULL;
}

void* consume(void* arg) {
    int id = *((int*)arg);
    int valeur;

    int base  = ELEM_PROD / nb_consommateur;
    int reste = ELEM_PROD % nb_consommateur;

    int items = base;
    if (id < reste) {
        items += 1;
    }

	for (int j=0; j<10000; j++);

	for (int i=0; i<items; i++) {
		monsem_wait(&full);
		lock(&mutex);
		//section critique

		valeur = buffer[position_consommateur];
		position_consommateur = (position_consommateur + 1) % TAILLE;
		printf("%d\n", valeur);
		cpt_cons = cpt_cons + 1;

		//section critique
		unlock(&mutex);
		monsem_post(&empty);
	}
	return NULL;
}


int main(int argc, char const *argv[])
{
	nb_producteur = atoi(argv[1]);
	nb_consommateur = atoi(argv[2]);
	int err;

	// initialisation
	pthread_t *thread_prod = malloc(nb_producteur * sizeof(pthread_t));
	pthread_t *thread_cons = malloc(nb_consommateur * sizeof(pthread_t));
	

	spinlock_init(&mutex);
	monsem_init(&empty, TAILLE);
	monsem_init(&full, 0);



	int *ids_prod = malloc(nb_producteur * sizeof(int));
	int *ids_cons = malloc(nb_consommateur * sizeof(int));
	for (int i = 0; i < nb_producteur; i++)
	{
		ids_prod[i] = i;
		err=pthread_create(&(thread_prod[i]), NULL, produce, &ids_prod[i]);
		if (err!=0)
			perror("pthread_create");
	}

	for (int i = 0; i < nb_consommateur; i++)
	{
		ids_cons[i] = i;
		err=pthread_create(&(thread_cons[i]), NULL, consume, &ids_cons[i]);
		if (err!=0)
			perror("pthread_create");
	}


	for (int i = 0; i < nb_producteur; i++)
	{
		pthread_join(thread_prod[i], NULL);
	}
	for (int i = 0; i < nb_consommateur; i++) {
    	pthread_join(thread_cons[i], NULL);
	}

	printf("valeur du cpt_prod : %d\n", cpt_prod);
	printf("valeur du cpt_cons : %d\n", cpt_cons);
	return 0;
}