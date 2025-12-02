#ifndef SPINLOCK_H
#define SPINLOCK_H

typedef struct {
    volatile int flag;  // 0 = libre, 1 = pris
} spinlock_t;

void spinlock_init(spinlock_t *lock);
void lock(spinlock_t *lock);
void unlock(spinlock_t *lock);

#endif
