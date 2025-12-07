/*
 * Implémentation d’un sémaphore minimaliste utilisant un spinlock
 * (verrou par attente active). Cette version ne met jamais les threads
 * en sommeil : elle repose uniquement sur un protocole de type
 * test-and-test-and-set (TTAS).
 *
 * Fonctionnement :
 *   - monsem_wait() : boucle active jusqu’à ce que value > 0, puis décrémente.
 *   - monsem_post() : incrémente value et débloque potentiellement un thread.
 *   - L’accès à value est protégé par un spinlock, ce qui garantit
 *     l’exclusion mutuelle tout en restant sans appel système.
 *
 * Utilité pédagogique :
 *   - Comprendre la différence entre sémaphores noyau et sémaphores
 *     purement utilisateurs basés sur un spinlock.
 *   - Mettre en évidence la consommation CPU due à l’attente active.
 */

#include "monsemaphore.h"

/*
 * Initialise un sémaphore :
 *   - value = init_val : nombre initial de ressources disponibles.
 *   - lock : spinlock protégeant l'accès à value.
 */
void monsem_init(monsemaphore_t *s, int init_val) {
    s->value = init_val;
    spinlock_init(&s->lock);
}

/*
 * Attente (P) :
 *   - Boucle infinie jusqu’à ce que value > 0.
 *   - Empêche l’entrée concurrente via un spinlock.
 *   - Implémente un comportement TTAS :
 *         * test : value > 0 ?
 *         * si non → pause → retest
 *
 * Ce sémaphore *ne met pas les threads en sommeil* : il consomme du CPU.
 */
void monsem_wait(monsemaphore_t *s) {
    while (1) {
        lock(&s->lock);               // Acquisition du spinlock

        if (s->value > 0) {           // Ressource disponible ?
            s->value--;               // Consomme une unité
            unlock(&s->lock);         // Libère l’accès
            return;                   // Sémaphore acquis
        }

        unlock(&s->lock);             // Libère avant de retenter

        // Instruction PAUSE : réduit la consommation lors du spin
        asm volatile("pause");
    }
}

/*
 * Signal (V) :
 *   - Incrémente value.
 *   - Protégé par le spinlock pour éviter une écriture concurrente.
 */
void monsem_post(monsemaphore_t *s) {
    lock(&s->lock);  
    s->value++;
    unlock(&s->lock);
}

