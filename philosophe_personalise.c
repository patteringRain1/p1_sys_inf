#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "spinlock.h"

#define NB_CYCLES 1000000

// Variables globales
spinlock_t *baguette;
int nb_philosophes;

void mange(int id)
{
    // printf pour debug (optionnel)
    // printf("Philosophe [%d] mange\n", id);
}

void *philosophe(void *arg)
{
    int id = *(int *)arg;
    int left = id;
    int right = (id + 1) % nb_philosophes;

    for (long cycle = 0; cycle < NB_CYCLES; cycle++)
    {
        if (left < right)
        {
            lock(&baguette[left]);
            lock(&baguette[right]);
        }
        else
        {
            lock(&baguette[right]);
            lock(&baguette[left]);
        }

        mange(id);

        unlock(&baguette[left]);
        unlock(&baguette[right]);
    }

    return NULL;
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s N_philosophes\n", argv[0]);
        return EXIT_FAILURE;
    }

    nb_philosophes = atoi(argv[1]);
    if (nb_philosophes <= 1)
    {
        fprintf(stderr, "N_philosophes doit être > 1\n");
        return EXIT_FAILURE;
    }

    baguette = malloc(sizeof(spinlock_t) * nb_philosophes);
    if (!baguette)
    {
        perror("malloc");
        return EXIT_FAILURE;
    }

    for (int i = 0; i < nb_philosophes; ++i)
        spinlock_init(&baguette[i]);

    pthread_t *threads = malloc(sizeof(pthread_t) * nb_philosophes);
    int *ids = malloc(sizeof(int) * nb_philosophes);

    if (!threads || !ids)
    {
        perror("malloc");
        return EXIT_FAILURE;
    }

    for (int i = 0; i < nb_philosophes; ++i)
    {
        ids[i] = i;
        if (pthread_create(&threads[i], NULL, philosophe, &ids[i]) != 0)
        {
            perror("pthread_create");
            return EXIT_FAILURE;
        }
    }

    for (int i = 0; i < nb_philosophes; ++i)
        pthread_join(threads[i], NULL);

    free(baguette);
    free(threads);
    free(ids);

    return EXIT_SUCCESS;
}
