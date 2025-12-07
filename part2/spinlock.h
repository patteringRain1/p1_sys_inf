#ifndef SPINLOCK_H
#define SPINLOCK_H

/*
 * Définition d’un spinlock minimaliste basé sur une variable
 * entière partagée (flag). Le spinlock repose sur une opération
 * atomique (xchg) implémentée dans spinlock.c, ce qui permet de
 * garantir l'exclusion mutuelle via une attente active.
 *
 * Convention :
 *   - flag = 0  → verrou libre
 *   - flag = 1  → verrou acquis
 *
 * Le verrou peut être implémenté en :
 *   - Test-and-Set (TAS)
 *   - Test-and-Test-and-Set (TTAS)
 *
 * Cette abstraction est utilisée par les différents modules du projet
 * (philosophes, lecteurs/écrivains, producteurs/consommateurs) afin
 * d'étudier l’impact de l’attente active sur la contention.
 */

/* Structure représentant un spinlock simple */
typedef struct {
    volatile int flag;   // 0 = libre, 1 = pris (accès concurrent protégé par xchg)
} spinlock_t;

/*
 * Initialise un spinlock :
 *   - flag = 0 → verrou disponible
 */
void spinlock_init(spinlock_t *lock);

/*
 * Acquire (lock) :
 *   - boucle active jusqu’à ce que flag passe à 0
 *   - le passage à 1 est atomique (via xchg)
 */
void lock(spinlock_t *lock);

/*
 * Release (unlock) :
 *   - remet flag à 0 de manière sûre
 */
void unlock(spinlock_t *lock);

#endif

