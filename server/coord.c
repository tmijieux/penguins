/**
 * @file coord.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <utils/log.h>
#include <server/coord.h>

/**
 * Calcule a à la puissance b.
 * @param a - Facteur.
 * @param b - Exposant.
 * @return int - a exposant b.
 */
static inline int powi(int a, int b)
{
    int p = 1;
    for (int i = 0; i < b; i++)
	p *= a;
    return p;
}


/**
 * Obtenir la longueur du bord du plateau.
 * @param number_tiles - Nombre de tuiles dans le jeu.
 * @param number_dimensions - Dimension du jeu : 1D, 2D, 3D, ...
 * @return size_t - Longueur du bord.
 */
size_t coord_get_length_side(int number_tiles, int number_dimensions)
{
    int a = 0;
    int b = number_tiles;
    while (b - a > 1) {
	int c = (a + b) / 2;
	if (powi(c, number_dimensions) >= number_tiles)
	    b = c;
	else
	    a = c;
    }
    return b;
}

/**
 * Obtenir les coordonnées d'une tuile en fonction de son identifiant.
 * @param id - Identifiant de la tuile. 
 * @param number_tiles - Nombre total de tuiles.
 * @param length - Taille de la tuile.
 * @param number_dimensions - Dimension du jeu : 1D, 2D, 3D, ...
 * @param coord - Coordonnées de la tuile.
 */
void coord_get_coordinates_from_id(int id, int number_tiles, size_t length,
				   int number_dimensions, int *coord)
{
    if (id >= number_tiles || id < 0) {
	log_print(NEVER_LOG__,
		  "Debug: get_coordinates_from_id: invalid id %d\n", id);
	coord[0] = -1;
	return;
    }
    int local_id = id;
    int cuboid = pow(length, number_dimensions - 1);
    for (int i = number_dimensions - 1; i >= 0; i--) {
	coord[i] = local_id / cuboid;
	local_id -= coord[i] * cuboid;
	cuboid /= length;
    }
}

/**
 * Obtenir l'identifiant d'une tuile en fonction de ses coordonnées.
 * @return int id - Identifiant de la tuile. 
 * @param coord - Coordonnées de la tuile.
 * @param length - Taille de la tuile.
 * @param number_dimensions - Dimension du jeu : 1D, 2D, 3D, ...
 */
int coord_get_id_from_coordinates(int *coord, size_t length,
				  int number_dimensions)
{
    int id = 0;
    int cuboid = pow(length, number_dimensions - 1);
    for (int i = number_dimensions - 1; i >= 0; i--) {
	if (coord[i] >= length || coord[i] < 0) {
	    log_print(NEVER_LOG__, "Debug: get_id_from_coordinates: "
		      "invalid coordinates %d\n", coord[i]);
	    return -1;
	}
	id += coord[i] * cuboid;
	cuboid /= length;
    }
    return id;
}
