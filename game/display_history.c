/**
 * @file record.c
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
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
static pthread_mutex_t _record_mut;

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
    struct record *rec = calloc(1, sizeof(*rec));
    rec->moves = calloc(capacity, sizeof(*rec->moves));
    rec->capacity = capacity;
    rec->size = 0;
    rec->pos = -1;
    rec->forward = 1;
    return rec;
}

/**
 * Libérer la mémoire occupée par le module d'enregistrement.
 * @param l - Le module d'enregistrement.
 */
void record_free(struct record *rec)
{
    free(rec->moves);
    memset(rec, 0, sizeof *rec);
    free(rec);
}

/**
 * Ajouter un enregistrement.
 * @param l - Le module d'enregistrement.
 * @param src - Source du mouvement.
 * @param dst - Destinationd du mouvement.
 * @return int - 1 Si l'ajout a fonctionné.
 */
int record_add(struct record *rec, int src, int dst)
{
    pthread_mutex_lock(&_record_mut);
    if (rec->size < rec->capacity) {
	rec->moves[rec->size].src = src;
	rec->moves[rec->size].dst = dst;
	++rec->size;
	pthread_mutex_unlock(&_record_mut);
	return 1;
    }
    pthread_mutex_unlock(&_record_mut);
    return 0;
}

/**
 * Avancer la lecture.
 * @param l - Le module d'enregistrement.
 * @return int - 1 Si le curseur a avancé.
 */
int record_next(struct record *rec)
{
    if (record_is_end(rec)) {
	return 0;
    }
    ++rec->pos;
    rec->forward = 1;
    return 1;
}

/**
 * Rembobiner la lecture.
 * @param l - Le module d'enregistrement.
 * @return int - 1 Si le curseur a reculé.
 */
int record_previous(struct record *rec)
{
    if (record_is_begin(rec)) {
	return 0;
    }
    --rec->pos;
    rec->forward = 0;
    return 1;
}

/**
 * Demander si la lecture est à la fin de l'enregistrement.
 * @param l - Le module d'enregistrement.
 * @return int - 1 Si c'est la fin.
 */
int record_is_end(struct record *rec)
{
    pthread_mutex_lock(&_record_mut);
    int b = rec->pos >= rec->size - 1;
    pthread_mutex_unlock(&_record_mut);
    return b;
}

/**
 * Demander si la lecture est au début de l'enregistrement.
 * @param l - Le module d'enregistrement.
 * @return int - 1 Si c'est le début.
 */
int record_is_begin(struct record *rec)
{
    return rec->pos < 0;
}

/**
 * Demander si la position du curseur est valide.
 * @param l - Le module d'enregistrement.
 * @return int - 1 Si valide.
 */
int record_pos_is_valid(struct record *rec)
{
    pthread_mutex_lock(&_record_mut);
    int b = rec->pos >= -1 && rec->pos < rec->size;
    pthread_mutex_unlock(&_record_mut);
    return b;
}

/**
 * Obtenir la source du mouvement en lecture.
 * @param l - Le module d'enregistrement.
 * @return int - Id de la tuile source.
 */
int record_get_current_src(struct record *rec)
{
    if (record_pos_is_valid(rec) && rec->forward)
	return rec->moves[rec->pos].src;
    return -1;
}

/**
 * Obtenir la destination du mouvement en lecture.
 * @param l - Le module d'enregistrement.
 * @return int - Id de la tuile de destination.
 */
int record_get_current_dest(struct record *rec)
{
    if (record_pos_is_valid(rec) && rec->forward)
	return rec->moves[rec->pos].dst;
    return -1;
}

/**
 * Obtenir la source du mouvement en lecture mode rewind.
 * @param l - Le module d'enregistrement.
 * @return int - Id de la tuile source.
 */
int record_get_rewind_src(struct record *rec)
{
    if (record_pos_is_valid(rec) && rec->forward == 0)
	return rec->moves[rec->pos + 1].dst;
    return -1;
}

/**
 * Obtenir la destination du mouvement en lecture mode rewind.
 * @param l - Le module d'enregistrement.
 * @return int - Id de la tuile de destination.
 */
int record_get_rewind_dest(struct record *rec)
{
    if (record_pos_is_valid(rec) && rec->forward == 0)
	return rec->moves[rec->pos + 1].src;
    return -1;
}
