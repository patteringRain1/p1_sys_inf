/*
 * Implémentation d’un spinlock utilisant l’algorithme
 * Test-and-Test-and-Set (TTAS).
 *
 * TTAS (Test → Test → Set) réduit la contention mémoire en séparant :
 *   1. Une boucle d’attente passive (lecture du flag en cache)
 *   2. Une tentative d’acquisition atomique via xchg
 *
 * Avantages :
 *   - Moins d’invalidations de cache que TAS pur
 *   - Meilleure scalabilité lorsque de nombreux threads tournent en parallèle
 *
 * Fonctionnement :
 *   - lock() :
 *         * lit lock->flag tant qu’il vaut 1 (attente locale en cache)
 *         * tente xchg(&flag,1) uniquement lorsque le verrou semble libre
 *   - unlock() :
 *         * remet flag à 0 de manière atomique
 */


#include "spinlock.h"

/*
 * xchg(ptr, val)
 * Échange atomiquement *ptr avec val et
 * retourne l’ancienne valeur de *ptr.
 *
 * L’instruction xchgl :
 *   - est atomique
 *   - invalide les copies cache des autres cœurs
 *   - agit comme barrière mémoire ("memory")
 */
static inline int xchg(volatile int *ptr, int val) {
    __asm__ volatile("xchgl %0, %1"
                 : "+r" (val), "+m" (*ptr)
                 :
                 : "memory");
    return val;
}

/* Initialise le spinlock : flag = 0 (libre) */
void spinlock_init(spinlock_t *lock) {
    lock->flag = 0;
}

/*
 * lock() — Algorithme TTAS (Test-and-Test-and-Set)
 *
 * Étapes :
 *   1. Attendre tant que le verrou est pris :
 *          while (lock->flag == 1) pause;
 *      → charge locale en cache, pas de xchg → très faible contention
 *
 *   2. Lorsque la valeur semble être 0, tenter xchg(&flag, 1)
 *      → si ancien flag = 0 → verrou acquis
 *      → sinon → recommencer
 *
 * Le "pause" :
 *   - réduit la consommation énergétique du CPU
 *   - améliore les performances dans les boucles de spin
 *   - évite un “busy-wait” agressif
 */
void lock(spinlock_t *lock) {

    while (1) {

        /* Phase “Test-and-Test” : simple lecture du flag */
        while (lock->flag == 1) {
            __asm__ volatile("pause");
        }

        /* Phase “Set” : vrai test atomique */
        if (xchg(&lock->flag, 1) == 0)
            return;  // Verrou acquis
    }
}


/*
 * unlock()
 * Libère le spinlock en remettant flag = 0
 * via un xchg atomique (pour garantir la visibilité immédiate).
 */
void unlock(spinlock_t *lock) {
    xchg(&lock->flag, 0);
}

