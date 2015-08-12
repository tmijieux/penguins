/**
 * @file map_hexagon.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <server/map.h>
#include <server/coord.h>
#include <display/display.h>

#define SQRT_3_2 0.86602540378

static int length;

/**
 * Initialisation du graphe.
 * @param dimension - Dimension du jeu : 1D, 2D, 3D, ...
 * @param nb_tile - Nombre de tuiles.
 * @param graph - Graphe du jeu.
 */
void map_init_graph_(int dimension, int nb_tile,
		     struct graph *graph)
{
    int *coord = malloc(dimension * sizeof(int));
    length = coord_get_length_side(nb_tile, dimension);

    #ifdef USE_GL_DISPLAY__
    struct model *m = NULL;
    struct texture *t = NULL;
    if (dimension >= 1 && dimension <= 3) {
	t = texture_load("textures/glace.jpg");
        m = model_load_wavefront("models/hexa_tile.obj");
	display_register_texture(t);
	display_register_model(m);
    }
    #endif

    for (int i = 0; i < nb_tile; i++) {
	int dim = length * length;
	coord_get_coordinates_from_id(i, nb_tile, length, 
				      dimension, coord);
        #ifdef USE_GL_DISPLAY__
	int fish = graph_get_fish(graph, i);
	switch (dimension) {
	case 1:
	    display_add_tile(i, m, t, - 0.5 * coord[0], 
			     0., coord[0] * SQRT_3_2, 0., 0.5, fish);   
	    break;
	case 2:
	    display_add_tile(i, m, t, coord[1] - 0.5 * coord[0],
			     0., coord[0] * SQRT_3_2, 0., 0.5, fish);
	    break;
	case 3:
	    display_add_tile(i, m, t, coord[1] - 0.5 * coord[0],
			     coord[2], coord[0] * SQRT_3_2, 0., 0.5, fish);
	    break;
	default:
	    break;
	}
	#endif
	
	if (coord[0] + 1 < length && i + 1 < nb_tile)
	    graph_add_edge(graph, i, i + 1);
	
	if (coord[1] + 1 < length && i + length < nb_tile)
	    graph_add_edge(graph, i, i + length);

	if (coord[0] + 1 < length && 
	    coord[1] + 1 < length && 
	    i + 1 + length < nb_tile)
	    graph_add_edge(graph, i, i + 1 + length);

	for (int j = 2; j < dimension; j++) {
	    if (coord[j] + 1 < length && i + dim < nb_tile)
		graph_add_edge(graph, i, i + dim);
	    dim *= length;
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
    if(dimension == 1)
	return 2;
    else
	return 2 + dimension * 2;
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
    int *coord = malloc(dimension * sizeof(int));
    int step = 1 - 2 * (*direction % 2);

    coord_get_coordinates_from_id(origin, nb_tile, length, dimension, coord);
    
    if (*direction < dimension * 2)
	coord[*direction / 2] += step;
    else { //Direction diagonale (2 dernières directions)	
	coord[0] += step;
	coord[1] += step;
    }

    int dest = coord_get_id_from_coordinates(coord, length, dimension);
    free(coord);
    return dest;
}

/**
 * Initialisation du graphe.
 * @param int - Dimension du jeu : 1D, 2D, 3D, ...
 * @param int - Nombre de tuiles.
 * @param struct graph * - Graphe du jeu.
 */
void (*map_init_graph) (int, int, struct graph *) =
    map_init_graph_;

/**
 * Obtenir le nombre de directions d'une tuile.
 * @param int - Identifiant de la tuile.
 * @param int - Dimension du jeu.
 * @param int - Nombre total de tuiles.
 * @return int - Nombre de directions.
 */
int (*map_get_number_directions) (int, int, int) =
    map_get_number_directions_;

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
int (*map_get_id_from_move) (int, int*, int, int, struct graph *) =
    map_get_id_from_move_;


