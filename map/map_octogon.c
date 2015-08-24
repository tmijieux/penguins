#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <server/map.h>
#include <server/coord.h>

#include <display.h>
#include <d3v/texture.h>
#include <d3v/model.h>

static int length;

/**
 * @file map_octogon.c
 */

/**
 * Indique la parité d'un nombre.
 * @param x - Nombre à tester.
 * @return int - 0 Pair, 1 Impair.
 */
static inline int odd(int x)
{
    return x % 2;
}

// odd => octogon
// even => square

/**
 * Initialisation du graphe.
 * @param dimension - Dimension du jeu : 1D, 2D, 3D, ...
 * @param nb_tile - Nombre de tuiles.
 * @param graph - Graphe du jeu.
 */
void map_init_graph_(int dimension, int nb_tile, struct graph *graph)
{
    int *coord = malloc(dimension * sizeof(int));
    length = coord_get_length_side(nb_tile, dimension);
    #ifdef USE_GL_DISPLAY__
    struct texture *t = NULL;
    struct model *octom = NULL, *sqm = NULL;
    if (dimension >= 1 && dimension <= 3) {
	t = texture_load("textures/glace.jpg");
        sqm = model_load_wavefront("models/square_tile.obj");
	octom = model_load_wavefront("models/octogon_tile.obj");
	display_register_texture(t);
	display_register_model(sqm);
	display_register_model(octom);
    }
    #endif
    for (int i = 0; i < nb_tile; i++) {
	int dim = 1;
	coord_get_coordinates_from_id(i, nb_tile, length,
				      dimension, coord);
	int odd_ = odd(coord[0] + coord[1]);
	
        #ifdef USE_GL_DISPLAY__
	int fish = graph_get_fish(graph, i);
	switch (dimension) {
	case 1:
	    display_add_tile(i, odd_ ? octom : sqm, t, 
			     0., 0., coord[0], 0.,
			     odd_ ? 0.4 : 0.25, fish);
	     break;
	 case 2:
	     display_add_tile(i, odd_ ? octom : sqm, t, 
			      coord[1], 0., coord[0], 0.,
			      odd_ ? 0.4 : 0.25, fish);
	     break;
	 case 3:
	     display_add_tile(i, odd_ ? octom : sqm, t, 
			      coord[1], coord[2], coord[0], 0.,
			      odd_ ? 0.4 : 0.25, fish);
	     break;
	 default:
	     break;
	 }
	 #endif
	 for (int j = 0; j < dimension; j++) {
	     if (coord[j] + 1 < length && i + dim < nb_tile)
		 graph_add_edge(graph, i, i + dim);
	     dim *= length;
	 }
	 if (odd_ && coord[0] + 1 < length) {
	     if (coord[1] + 1 < length && i + length + 1 < nb_tile) 
		 graph_add_edge(graph, i, i + length + 1);
	     if (coord[1] - 1 >= 0 && i - length + 1 >= 0)
		 graph_add_edge(graph, i, i - length + 1);
	 }
     }
     free(coord);
 }

/**
 * Obtenir le nombre de directions d'une tuile.
 * @param tile - Identifiant de la tuile.
 * @param dimension - Dimension du jeu.
 * @param nb_tile - Nombre total de tuiles.
 * @return int - Nombre de directions.
 */
int map_get_number_directions_(int tile, int dimension, int nb_tile)
 {
     if (dimension == 1)
	 return 2;

     int *coord = malloc(dimension * sizeof(int));
     coord_get_coordinates_from_id(tile, nb_tile, length, dimension, coord);
     int odd_ = odd(coord[0] + coord[1]);
     free(coord);
     return dimension * 2 + 4 * odd_;
 }

/**
 * Obtenir la destination d'un mouvement.
 * @param origin - Origine du mouvement (identifiant de la tuile).
 * @param direction - Direction du mouvement. Le mappeur peut se permettre 
 * de changer sa valeur pour indiquer au serveur quelle est la prochaine 
 * direction à prendre pour avoir  un déplacement en ligne droite.
 * @param dimension - Dimension du jeu.
 * @param nb_tile - Nombre total de tuiles.
 * @param graph - Graphe du jeu.
 * @return int - Destination du mouvment
 */
int map_get_id_from_move_(int origin, int *direction, int dimension,
			   int nb_tile, struct graph *graph)
 {
     int *coord = malloc(dimension * sizeof(*coord));
     coord_get_coordinates_from_id(origin, nb_tile, length, dimension, coord);
     int odd_ = odd(coord[0] + coord[1]);
     int dest;

     if (*direction < dimension * 2) {	
	 coord[*direction / 2] += 1 - 2 * (*direction % 2);
	 dest = coord_get_id_from_coordinates(coord, length, dimension);
     } else if (odd_ && *direction < (dimension * 2 + 4)) { // diagonal direction
	 int diag_direction = *direction - 2 * dimension;
	 coord[0] += 1 - 2 * (diag_direction % 2);
	 coord[1] += 1 - 2 * (diag_direction < 2);
	 dest = coord_get_id_from_coordinates(coord, length, dimension);
    } else
	dest = -1;
 
    free(coord);
    return dest;
}

/**
 * Initialisation du graphe.
 * @param int - Dimension du jeu : 1D, 2D, 3D, ...
 * @param int - Nombre de tuiles.
 * @param struct graph * - Graphe du jeu.
 */
void (*map_init_graph) (int, int, struct graph*) =
    &map_init_graph_;

/**
 * Obtenir le nombre de directions d'une tuile.
 * @param int - Identifiant de la tuile.
 * @param int - Dimension du jeu.
 * @param int - Nombre total de tuiles.
 * @return int - Nombre de directions.
 */
int (*map_get_number_directions) (int, int, int) =
    &map_get_number_directions_;

/**
 * Obtenir la destination d'un mouvement.
 * @param int - Origine du mouvement (identifiant de la tuile).
 * @param int * - Direction du mouvement.Le mappeur peut se permettre 
 * de changer sa valeur pour indiquer au serveur quelle est la prochaine 
 * direction à prendre pour avoir  un déplacement en ligne droite.
 * @param int - Dimension du jeu.
 * @param int - Nombre total de tuiles.
 * @param struct graph * - Graphe du jeu.
 * @return int - Destination du mouvment
 */
int (*map_get_id_from_move) (int, int*, int, int, struct graph*) =
    &map_get_id_from_move_;

