#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "spinlock.h"

#define TOTAL_SECTIONS_CRITIQUES 32768

spinlock_t spinlock;
volatile int cpt=0;
int N;

void* section_critique(void* arg) {
    int items = TOTAL_SECTIONS_CRITIQUES / N;

    for (int j=0; j <10000; j++);

    for (int i = 0; i < items; i++) {
        lock(&spinlock);
        // section critique

        cpt++;

        // section critique
        unlock(&spinlock);
    }
    return NULL;
}

int main(int argc, char *argv[]) {

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    N = atoi(argv[1]);
    int err;

    // initialisation
    pthread_t threads[N];
    spinlock_init(&spinlock);


    for (int i = 0; i < N; i++) {
        err=pthread_create(&(threads[i]), NULL, section_critique, NULL);
        if (err!=0)
            perror("pthread_create");
    }

    for (int i = 0; i < N; i++) {
        pthread_join(threads[i], NULL);
    }


    printf("valeur du cpt : %d\n", cpt);


    clock_gettime(CLOCK_MONOTONIC, &end);
    double temps = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Temps d'execution : %f secondes\n", temps);

    return 0;
}
