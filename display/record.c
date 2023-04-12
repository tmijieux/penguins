/**
 * @file record.c
 */

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include "display/record.h"

/**
 * Description d'un mouvement.
 */
struct dmove {
    int src;
    int dst;
};

// Mutex pour le thread.
static pthread_mutex_t m;

struct record {
    struct dmove *moves;
    int capacity;
    int size;
    int pos;
    int forward;
};

/**
 * Création du module d'enregistrement des mouvements.
 * @param capacity - Capacité total d'enregistrement.
 * @return struct record * - Le module d'enregistrement.
 */
struct record *record_create(int capacity)
{
    struct record *l = malloc(sizeof(struct record));
    l->moves = malloc(sizeof(struct dmove) * capacity);
    l->capacity = capacity;
    l->size = 0;
    l->pos = -1;
    l->forward = 1;
    return l;
}

/**
 * Libérer la mémoire occupée par le module d'enregistrement.
 * @param l - Le module d'enregistrement.
 */
void record_free(struct record *l)
{
    free(l->moves);
    free(l);
}

/**
 * Ajouter un enregistrement.
 * @param l - Le module d'enregistrement.
 * @param src - Source du mouvement.
 * @param dst - Destinationd du mouvement.
 * @return int - 1 Si l'ajout a fonctionné.
 */
int record_add(struct record *l, int src, int dst)
{
    pthread_mutex_lock(&m);
    if (l->size < l->capacity) {
	l->moves[l->size].src = src;
	l->moves[l->size].dst = dst;
	++l->size;
	pthread_mutex_unlock(&m);
	return 1;
    }
    pthread_mutex_unlock(&m);
    return 0;
}

/**
 * Avancer la lecture.
 * @param l - Le module d'enregistrement.
 * @return int - 1 Si le curseur a avancé.
 */
int record_next(struct record *l)
{
    if (record_is_end(l))
	return 0;
    ++l->pos;
    l->forward = 1;
    return 1;
}

/**
 * Rembobiner la lecture.
 * @param l - Le module d'enregistrement.
 * @return int - 1 Si le curseur a reculé.
 */
int record_previous(struct record *l)
{
    if (record_is_begin(l))
	return 0;
    --l->pos;
    l->forward = 0;
    return 1;
}

/**
 * Demander si la lecture est à la fin de l'enregistrement.
 * @param l - Le module d'enregistrement.
 * @return int - 1 Si c'est la fin.
 */
int record_is_end(struct record *l)
{
    pthread_mutex_lock(&m);
    int b = l->pos >= l->size - 1;
    pthread_mutex_unlock(&m);
    return b;
}

/**
 * Demander si la lecture est au début de l'enregistrement.
 * @param l - Le module d'enregistrement.
 * @return int - 1 Si c'est le début.
 */
int record_is_begin(struct record *l)
{
    return l->pos < 0;
}

/**
 * Demander si la position du curseur est valide.
 * @param l - Le module d'enregistrement.
 * @return int - 1 Si valide.
 */
int record_pos_is_valid(struct record *l)
{
    pthread_mutex_lock(&m);
    int b = l->pos >= -1 && l->pos < l->size;
    pthread_mutex_unlock(&m);
    return b;
}

/**
 * Obtenir la source du mouvement en lecture.
 * @param l - Le module d'enregistrement.
 * @return int - Id de la tuile source.
 */
int record_get_current_src(struct record *l)
{
    if (record_pos_is_valid(l) && l->forward)
	return l->moves[l->pos].src;
    return -1;
}

/**
 * Obtenir la destination du mouvement en lecture.
 * @param l - Le module d'enregistrement.
 * @return int - Id de la tuile de destination.
 */
int record_get_current_dest(struct record *l)
{
    if (record_pos_is_valid(l) && l->forward)
	return l->moves[l->pos].dst;
    return -1;
}

/**
 * Obtenir la source du mouvement en lecture mode rewind.
 * @param l - Le module d'enregistrement.
 * @return int - Id de la tuile source.
 */
int record_get_rewind_src(struct record *l)
{
    if (record_pos_is_valid(l) && l->forward == 0)
	return l->moves[l->pos + 1].dst;
    return -1;
}

/**
 * Obtenir la destination du mouvement en lecture mode rewind.
 * @param l - Le module d'enregistrement.
 * @return int - Id de la tuile de destination.
 */
int record_get_rewind_dest(struct record *l)
{
    if (record_pos_is_valid(l) && l->forward == 0)
	return l->moves[l->pos + 1].src;
    return -1;
}
