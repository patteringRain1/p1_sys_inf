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
	int items = NOMBRE_ECRITURE / nb_ecrivain;
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
	int items = NOMBRE_LECTURE / nb_lecteur;

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
		nb_lecture_reel++;

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

	for (int i = 0; i < nb_ecrivain; i++)
	{
		err= pthread_create(&thread_ecri[i], NULL, writer, NULL);
		if (err != 0)
			perror("pthread_create");
	}

	for (int i = 0; i < nb_lecteur; i++)
	{
		err = pthread_create(&thread_lect[i], NULL, reader, NULL);
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