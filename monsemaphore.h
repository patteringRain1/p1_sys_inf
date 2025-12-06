#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include "spinlock.h"

typedef struct {
    int value;
    spinlock_t lock;
} monsemaphore_t;

void monsem_init(monsemaphore_t *s, int init_val);
void monsem_wait(monsemaphore_t *s);
void monsem_post(monsemaphore_t *s);

#endif
