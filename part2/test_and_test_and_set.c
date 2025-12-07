#include "spinlock.h"

static inline int xchg(volatile int *ptr, int val) {
    __asm__ volatile("xchgl %0, %1"
                 : "+r" (val), "+m" (*ptr)
                 :
                 : "memory");
    return val;
}

void spinlock_init(spinlock_t *lock) {
    lock->flag = 0;
}

void lock(spinlock_t *lock) {

    while (1) {
        while (lock->flag == 1) {
            __asm__ volatile("pause");
        }

        if (xchg(&lock->flag, 1) == 0)
            return;
    }
}

void unlock(spinlock_t *lock) {
    xchg(&lock->flag, 0);
}
