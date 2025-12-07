#ifndef SEMAPHORE_H
#define SEMAPHORE_H

/*
 * Définition d’un sémaphore minimaliste implémenté entièrement
 * en espace utilisateur, basé sur un spinlock (attente active).
 *
 * Fonctionnement :
 *   - value : nombre de ressources disponibles.
 *   - lock  : spinlock protégeant l’accès à value.
 *
 * Ce sémaphore ne met jamais les threads en sommeil :
 * il repose sur une boucle d’attente active (type TTAS),
 * ce qui le rend utile pour un environnement pédagogique ou très léger.
 */

#include "spinlock.h"

/* Structure représentant un sémaphore maison */
typedef struct {
    int value;          // Compteur interne (≥0)
    spinlock_t lock;    // Verrou par attente active protégeant value
} monsemaphore_t;

/*
 * Initialise le sémaphore avec une valeur initiale.
 * init_val > 0 signifie que des ressources sont disponibles.
 */
void monsem_init(monsemaphore_t *s, int init_val);

/*
 * Opération P (wait) :
 *   - Attend qu’une ressource soit disponible
 *   - Décrémente value de manière atomique
 *   - Bloque en attente active tant que value == 0
 */
void monsem_wait(monsemaphore_t *s);

/*
 * Opération V (post) :
 *   - Libère une ressource
 *   - Incrémente value de manière atomique
 */
void monsem_post(monsemaphore_t *s);

#endif

