/**
 * @file record.c
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef __unix__
#include <pthread.h>
#else
#include <Windows.h>
#endif

#include "display/record.h"

/**
 * Description d'un mouvement.
 */
typedef struct dmove {
    int src;
    int dst;
} dmove_t;

// Mutex pour le thread.
#ifdef __unix__
static pthread_mutex_t _record_mut = PTHREAD_MUTEX_INITIALIZER;
#else
int mutex_initialized = 0;
CRITICAL_SECTION _record_mut;
#endif

struct record {
    int capacity;
    int size;
    int pos;
    int forward;
    dmove_t *moves;
};

/**
 * Création du module d'enregistrement des mouvements.
 * @param capacity - Capacité total d'enregistrement.
 * @return record_t * - Le module d'enregistrement.
 */
record_t *record_create(int capacity)
{
#ifndef __unix__
    if (!mutex_initialized) {
        mutex_initialized = 1;
        InitializeCriticalSection(&_record_mut);
    }
#endif

    record_t *rec = calloc(1, sizeof(*rec));
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
void record_free(record_t *rec)
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
int record_add(record_t *rec, int src, int dst)
{
#ifdef __unix__
    pthread_mutex_lock(&_record_mut);
#else
    EnterCriticalSection(&_record_mut);
#endif
    if (rec->size < rec->capacity) {
        rec->moves[rec->size].src = src;
        rec->moves[rec->size].dst = dst;
        ++rec->size;
#ifdef __unix__
        pthread_mutex_unlock(&_record_mut);
#else
        LeaveCriticalSection(&_record_mut);
#endif
        return 1;
    }

#ifdef __unix__
    pthread_mutex_unlock(&_record_mut);
#else
    LeaveCriticalSection(&_record_mut);
#endif
    return 0;
}

/**
 * Avancer la lecture.
 * @param l - Le module d'enregistrement.
 * @return int - 1 Si le curseur a avancé.
 */
int record_next(record_t *rec)
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
int record_previous(record_t *rec)
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
int record_is_end(record_t *rec)
{
#ifdef __unix__
    pthread_mutex_lock(&_record_mut);
    int b = rec->pos >= rec->size - 1;
    pthread_mutex_unlock(&_record_mut);
#else
    EnterCriticalSection(&_record_mut);
    int b = rec->pos >= rec->size - 1;
    LeaveCriticalSection(&_record_mut);
#endif
    return b;
}

/**
 * Demander si la lecture est au début de l'enregistrement.
 * @param l - Le module d'enregistrement.
 * @return int - 1 Si c'est le début.
 */
int record_is_begin(record_t *rec)
{
    return rec->pos < 0;
}

/**
 * Demander si la position du curseur est valide.
 * @param l - Le module d'enregistrement.
 * @return int - 1 Si valide.
 */
int record_pos_is_valid(record_t *rec)
{
#ifdef __unix__
    pthread_mutex_lock(&_record_mut);
    int b = rec->pos >= -1 && rec->pos < rec->size;
    pthread_mutex_unlock(&_record_mut);
#else
    EnterCriticalSection(&_record_mut);
    int b = rec->pos >= -1 && rec->pos < rec->size;
    LeaveCriticalSection(&_record_mut);
#endif
    return b;
}

/**
 * Obtenir la source du mouvement en lecture.
 * @param l - Le module d'enregistrement.
 * @return int - Id de la tuile source.
 */
int record_get_current_src(record_t *rec)
{
    if (record_pos_is_valid(rec) && rec->forward) {
        return rec->moves[rec->pos].src;
    }
    return -1;
}

/**
 * Obtenir la destination du mouvement en lecture.
 * @param l - Le module d'enregistrement.
 * @return int - Id de la tuile de destination.
 */
int record_get_current_dest(record_t *rec)
{
    if (record_pos_is_valid(rec) && rec->forward) {
        return rec->moves[rec->pos].dst;
    }
    return -1;
}

/**
 * Obtenir la source du mouvement en lecture mode rewind.
 * @param l - Le module d'enregistrement.
 * @return int - Id de la tuile source.
 */
int record_get_rewind_src(record_t *rec)
{
    if (record_pos_is_valid(rec) && rec->forward == 0) {
        return rec->moves[rec->pos + 1].dst;
    }
    return -1;
}

/**
 * Obtenir la destination du mouvement en lecture mode rewind.
 * @param l - Le module d'enregistrement.
 * @return int - Id de la tuile de destination.
 */
int record_get_rewind_dest(record_t *rec)
{
    if (record_pos_is_valid(rec) && rec->forward == 0) {
        return rec->moves[rec->pos + 1].src;
    }
    return -1;
}
