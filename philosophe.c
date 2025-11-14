#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

pthread_t phil[PHILOSOPHES];
pthread_mutex_t baguette[PHILOSOPHES];

void mange(int id)
{
    printf("Philosophe [%d] mange\n", id);
    for (int i = 0; i < rand(); i++)
    {
        // philosophe mange
    }
}

void *philosophe(void *arg)
{
    int *id = (int *)arg;
    int left = *id;
    int right = (left + 1) % PHILOSOPHES;
    while (true)
    {
        if (left < right)
        {
            pthread_mutex_lock(&baguette[left]);
            pthread_mutex_lock(&baguette[right]);
        }
        else
        {
            pthread_mutex_lock(&baguette[right]);
            pthread_mutex_lock(&baguette[left]);
        }
        mange(*id);
        pthread_mutex_unlock(&baguette[left]);
        pthread_mutex_unlock(&baguette[right]);
    }
    return (NULL);
}