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


void* writer (void* arg) {
	int id = *((int*) arg);
	int base = NOMBRE_ECRITURE / nb_ecrivain;
	int reste = NOMBRE_ECRITURE % nb_ecrivain;

	int items = base;
	if (id < reste) {
		items += 1;
	}

	for (int i = 0; i <items; i++) {
		sem_wait(&readtry);
		sem_wait(&wrt);
		//section critique

		for (int j = 0; j < 10000; j++);
		nb_ecriture_reel++;

		//section critique
		sem_post(&wrt);
		sem_post(&readtry);
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
		sem_wait(&readtry);
		sem_wait(&mutex);
		//section critique

		readcount++;
		if (readcount == 1)
			sem_wait(&wrt);

		//section critique
		sem_post(&mutex);
		sem_post(&readtry);

		for (int j = 0; j< 10000; j++);
		//section critique

		sem_wait(&mutex);
		nb_lecture_reel++;
		sem_post(&mutex);

		//section critique
		sem_wait(&mutex);
		//section critique

		readcount--;
		if (readcount == 0)
			sem_post(&wrt);

		//section critique
		sem_post(&mutex);
	}
	return NULL;
}



int main(int argc, char const *argv[])
{

	nb_ecrivain = atoi(argv[1]);
	nb_lecteur = atoi(argv[2]);
	int err;

	// initialisation
	pthread_t thread_ecri[nb_ecrivain];
	pthread_t thread_lect[nb_lecteur];

	err = sem_init(&mutex, 0, 1);
	if (err!=0)
		perror("sem_init_mutex");

	err = sem_init(&wrt, 0, 1);
	if (err!=0)
		perror("sem_init_wrt");

	err = sem_init(&readtry, 0, 1);
	if (err!=0)
		perror("sem_init_readtry");

	int ids_ecri[nb_ecrivain];
	for (int i = 0; i < nb_ecrivain; i++)
	{
		ids_ecri[i] = i;
		err= pthread_create(&thread_ecri[i], NULL, writer, &ids_ecri[i]);
		if (err != 0)
			perror("pthread_create");
	}

	int ids_lect[nb_lecteur];
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
	sem_destroy(&mutex);
	sem_destroy(&wrt);
	sem_destroy(&readtry);

	printf("valeur du nb_ecriture_reel : %d\n", nb_ecriture_reel);
	printf("valeur du nb_lecture_reel : %d\n", nb_lecture_reel);

	return 0;
}