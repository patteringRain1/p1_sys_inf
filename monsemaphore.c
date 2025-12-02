#include "monsemaphore.h"

void sem_init(monsemaphore_t *s, int init_val) {
    s->value = init_val;
    spinlock_init(&s->lock);
}

void monsem_wait(semaphore_t *s) {
    while (true) {
        lock(&s->lock);

        if (s->value > 0) {
            s->value--;
            unlock(&s->lock);
            return;
        }

        unlock(&s->lock);
        asm volatile("pause");
    }
}

void monsem_post(semaphore_t *s) {
    lock(&s->lock);
    s->value++;
    unlock(&s->lock);
}
