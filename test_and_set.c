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
    while (xchg(&lock->flag, 1) == 1) {
        while (lock->flag) {
            __asm__ volatile("pause");
        }
    }
}


void unlock(spinlock_t *lock) {
    xchg(&lock->flag, 0);
}