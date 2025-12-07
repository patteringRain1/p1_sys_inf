#include "monsemaphore.h"

void monsem_init(monsemaphore_t *s, int init_val) {
    s->value = init_val;
    spinlock_init(&s->lock);
}

void monsem_wait(monsemaphore_t *s) {
    while (1) {
        lock(&s->lock);

        if (s->value > 0) {
            s->value--;
            unlock(&s->lock);
            return;
        }

        unlock(&s->lock);
        __asm__ volatile("pause");
    }
}

void monsem_post(monsemaphore_t *s) {
    lock(&s->lock);
    s->value++;
    unlock(&s->lock);
}
