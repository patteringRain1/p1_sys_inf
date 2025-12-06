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


void* writer (void* arg) {
	int id = *((int*) arg);
	int base = NOMBRE_ECRITURE / nb_ecrivain;
	int reste = NOMBRE_ECRITURE % nb_ecrivain;

	int items = base;
	if (id < reste) {
		items += 1;
	}

	for (int i = 0; i <items; i++) {
		monsem_wait(&readtry);
		monsem_wait(&wrt);
		//section critique

		for (int j = 0; j < 10000; j++);
		nb_ecriture_reel++;

		//section critique
		monsem_post(&wrt);
		monsem_post(&readtry);
	}
	return NULL;
}

void* reader (void* arg) {
	int id = *((int*) arg);
	int base = NOMBRE_LECTURE / nb_lecteur;
	int reste = NOMBRE_LECTURE % nb_lecteur;

	int items = base;
	if (id < reste) {
		items += 1;
	}

	for (int i = 0; i <items; i++) {
		monsem_wait(&readtry);
		monsem_wait(&mutex);
		//section critique

		readcount++;
		if (readcount == 1)
			monsem_wait(&wrt);

		//section critique
		monsem_post(&mutex);
		monsem_post(&readtry);

		for (int j = 0; j< 10000; j++);
		//section critique

		monsem_wait(&mutex);
		nb_lecture_reel++;
		monsem_post(&mutex);

		//section critique
		monsem_wait(&mutex);
		//section critique

		readcount--;
		if (readcount == 0)
			monsem_post(&wrt);

		//section critique
		monsem_post(&mutex);
	}
	return NULL;
}



int main(int argc, char const *argv[])
{
	nb_ecrivain = atoi(argv[1]);
	nb_lecteur = atoi(argv[2]);
	int err;

	// initialisation
	pthread_t *thread_ecri = malloc(nb_ecrivain * sizeof(pthread_t));
	pthread_t *thread_lect = malloc(nb_lecteur * sizeof(pthread_t));

	monsem_init(&mutex, 1);

	monsem_init(&wrt, 1);

	monsem_init(&readtry, 1);


	int *ids_ecri = malloc(nb_ecrivain * sizeof(int));
	for (int i = 0; i < nb_ecrivain; i++)
	{
		ids_ecri[i] = i;
		err= pthread_create(&thread_ecri[i], NULL, writer, &ids_ecri[i]);
		if (err != 0)
			perror("pthread_create");
	}

	int *ids_lect = malloc(nb_lecteur * sizeof(int));
	for (int i = 0; i < nb_lecteur; i++)
	{
		ids_lect[i] = i;
		err = pthread_create(&thread_lect[i], NULL, reader, &ids_lect[i]);
		if (err != 0)
			perror("pthread_create");
	}



	for (int i = 0; i < nb_ecrivain; i++)
	{
		pthread_join(thread_ecri[i], NULL);
	}

	for (int i = 0; i < nb_lecteur; i++)
	{
		pthread_join(thread_lect[i], NULL);
	}


	// destruction
	free(thread_ecri);
	free(thread_lect);
	free(ids_ecri);
	free(ids_lect);

	printf("nombre de lecture : %d\n", nb_lecture_reel);
	printf("nombre de écriture : %d\n", nb_ecriture_reel);

	return 0;
}
