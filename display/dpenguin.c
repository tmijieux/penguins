/**
 * @file dpenguin.c
 */

#include <stdlib.h>

#include <utils/vec.h>

#include <display/dpenguin.h>
#include <d3v/object.h>
#include <d3v/texture.h>
#include <d3v/model.h>

/**
 * Représentation d'un pingouin.
 */
struct dpenguin {
    struct object *obj;
};

/**
 * Créé un pingouin.
 * @param m - Model du pingouin.
 * @param t - Texture du pingouin.
 * @param pos - Position du pingouin.
 * @param rot - Orientation du pingouin.
 * @param scale - Echelle du pingouin.
 * @return struct dpenguin * - Le pingouin.
 */
struct dpenguin*
dpenguin_create(struct model *m, struct texture *t,
		vec3 pos, double rot, double scale)
{
    struct dpenguin *pen = malloc(sizeof(*pen));
    vec3 sca = { scale, scale, scale };
    vec3 rot__ = { 0., rot, 0. };
    pen->obj = d3v_object_create(m, t, pos, rot__, sca);
    return pen;
}

/**
 * Dessiner le pinguoin. 
 * @param pen - Le pingouin.
 */
void dpenguin_draw(struct dpenguin *pen)
{
    d3v_object_draw(pen->obj);
}

/**
 * Obtenir l'orientation du pingouin.
 * @param pen - Le pingouin.
 * @return double - Orientation en degré.
 */
double dpenguin_get_orientation(struct dpenguin *pen)
{
    vec3 orient;
    d3v_object_get_orientation(pen->obj, &orient);
    return orient.y;
}

/**
 * Effectuer la rotation du pingouin.
 * @param pen - Le pingouin.
 * @param a - Angle de rotation.
 */
void dpenguin_rotate(struct dpenguin *pen, double a)
{
    d3v_object_set_orientation(pen->obj, (vec3) { 0., a, 0. });
}
		
/**
 * Obtenir la position du pingouin.
 * @param pen - Le pingouin.
 * @param pos - Position du pingouin.
 */
void dpenguin_get_position(struct dpenguin *pen, vec3 *pos)
{
    d3v_object_get_position(pen->obj, pos);
}

/**
 * Changer la position du pingouin.
 * @param pen - Le pingouin.
 * @param pos - Nouvelle position.
 */
void dpenguin_set_position(struct dpenguin *pen, vec3 pos)
{
    d3v_object_set_position(pen->obj, pos);
}

/**
 * Permet savoir si le pingouin est sur une tuile.
 * @param pen - Le pingouin.
 * @param ti - Tuile à tester.
 * @return int - 1 si le pingouin est sur la tuile ti.
 */
int dpenguin_is_on_tile(struct dpenguin *pen, struct dtile* ti)
{
    vec3 pos;
    dtile_get_position(ti, &pos);
    return d3v_object_pos_equal(pen->obj, pos);
}

/**
 * Cacher le pingouin.
 * @param pen - Le pingouin.
 */
void dpenguin_hide(struct dpenguin *pen)
{
    d3v_object_hide(pen->obj);
}

/**
 * Révéler le pingouin.
 * @param pen - Le pingouin.
 */
void dpenguin_reveal(struct dpenguin *pen)
{
    d3v_object_reveal(pen->obj);
}

/**
 * Libérer la mémoire occupée par le pingouin.
 * @param pen - Le pingouin.
 */
void dpenguin_free(struct dpenguin *pen)
{
    if (pen == NULL)
	return;
    d3v_object_free(pen->obj);
    free(pen);
}
