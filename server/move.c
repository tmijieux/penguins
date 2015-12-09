/**
 * @file move.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <server/move.h>
#include <server/tile.h>
#include <server/server.h>
#include <server/map.h>
#include <utils/log.h>
#include <utils/graph.h>

/* 
 *   BEGIN ( INTERFACE )
 *     --> (dont touch the names)
 *     --> (dont touch the headers)
 */

/**
 * Obtenir l'origine d'un mouvement.
 * @param move - Le mouvement.
 * @return int - Origine du mouvement.
 */
int move__get_orig(struct move *move)
{
    return move->orig;
}

/**
 * Obtenir la direction d'un mouvement.
 * @param move - Le mouvement.
 * @return int - Direction du mouvement.
 */
int move__get_direction(struct move *move)
{
    return move->direction;
}

/**
 * Obtenir le nombre de sauts d'un mouvement.
 * @param move - Le mouvement.
 * @return int - Nombre de sauts du mouvement.
 */
int move__get_nb_jump(struct move *move)
{
    return move->jump_count;
}

/**
 * Enregistrer les valeurs dans un mouvement.
 * @param move - Le mouvement.
 * @param tile_id_orig - Origine du mouvement.
 * @param direction - Direction du mouvement.
 * @param nb_move - Nombre de sauts du mouvement.
 */
void move_set(struct move *move, int tile_id_orig,
	      int direction, int nb_move)
{
    move->orig = tile_id_orig;
    move->direction = direction;
    move->jump_count = nb_move;
}

/* 
 *   END ( INTERFACE )
 *     --> (I hope you did not touch the names! (better for you you didn't))
 *     --> (If you touched the headers: your head's off !)
 */
