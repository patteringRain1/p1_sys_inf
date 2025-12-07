/*
 * Implémentation d’un spinlock simple basé sur l’instruction
 * atomique xchg de l’architecture x86.
 *
 * Fonctionnement :
 *   - xchg(&flag, 1) renvoie l’ancienne valeur du flag et place 1 en mémoire :
 *         * si l’ancienne valeur = 0 → verrou acquis
 *         * si l’ancienne valeur = 1 → verrou occupé
 *
 *   - lock() utilise l’algorithme Test-and-Test-and-Set (TTAS) :
 *         1. tant que le verrou est pris, on lit lock->flag en boucle
 *            (attente locale en cache → réduit le trafic mémoire)
 *         2. lorsque flag semble libre, on tente un xchg atomique
 *         3. si xchg échoue, on recommence la boucle
 *
 *   - unlock() remet simplement le flag à 0 via un xchg, garantissant
 *     une écriture atomique visible par tous les threads.
 *
 * Intérêt pédagogique :
 *   - Montrer la différence entre TAS et TTAS
 *   - Visualiser l’impact des invalidations de cache dues à xchg
 *   - Étudier l'attente active et ses performances
 */


#include "spinlock.h"

/*
 * xchg(ptr, val)
 * Effectue un échange atomique :
 *   - remplace *ptr par val
 *   - retourne l’ancienne valeur
 *
 * L’instruction xchgl garantit :
 *   - atomicité (bus locked)
 *   - invalidation du cache sur les autres cœurs
 *
 * Contraintes :
 *   - "+r" (val) : val est lu/écrit
 *   - "+m" (*ptr) : la mémoire pointée est modifiée
 *   - "memory" : barrière interdisant la réordonnation
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
 * lock()
 * Implémente un spinlock avec l’algorithme Test-and-Test-and-Set (TTAS).
 *
 * Étapes :
 *   1. xchg(&flag, 1)
 *        → si flag = 0 → verrou acquis
 *        → sinon → on entre dans la boucle d’attente
 *
 *   2. while (lock->flag)
 *        → attente passive sur la copie locale du cache
 *        → évite de saturer le bus mémoire avec des xchg répétés
 *
 * Le "pause" :
 *   - instruction spéciale x86 réduisant la consommation dans une boucle de spin
 *   - améliore les performances dans un contexte multi-cœur
 */
void lock(spinlock_t *lock) {
    while (xchg(&lock->flag, 1) == 1) {  // Tentative d’acquisition du verrou
        while (lock->flag) {            // Attente tant que le verrou est pris
            __asm__ volatile("pause");  // Hint CPU → moins d’énergie / meilleurs perfs HT
        }
    }
}


/*
 * unlock()
 * Libère le verrou en remettant flag à 0 via un xchg atomique.
 *
 * Pourquoi xchg ?
 *   - garantit que l’écriture est visible immédiatement
 *   - évite toute race condition avec des threads qui testent le flag
 */
void unlock(spinlock_t *lock) {
    xchg(&lock->flag, 0);
}

