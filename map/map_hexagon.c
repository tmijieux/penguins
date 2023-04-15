/**
 * @file map_hexagon.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "map_interface.h"

#include "server/map.h"
#include "server/coord.h"
#include "display/display.h"

#define SQRT_3_2 0.86602540378

static int length;

/**
 * Initialisation du graphe.
 * @param dimension - Dimension du jeu : 1D, 2D, 3D, ...
 * @param nb_tile - Nombre de tuiles.
 * @param graph - Graphe du jeu.
 */
static void init_graph(int dimension, int nb_tile, graph_t *graph)
{
    int *coord = malloc(dimension * sizeof(int));
    length = coord_get_length_side(nb_tile, dimension);

    int model = -1;
    int texture = -1;
    if (dimension >= 1 && dimension <= 3) {
	texture = display_register_texture("textures/glace.jpg");
        model = display_register_model("models/wavefront/hexa_tile.obj");
    }

    for (int i = 0; i < nb_tile; i++) {
	int dim = length * length;
	coord_get_coordinates_from_id(i, nb_tile, length,
				      dimension, coord);

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

        vdata_t data;
	graph_get_data(graph, i, &data);
        data.model_id = model;
        data.texture_id = texture;
        data.angle = 0;
        data.scale = 0.5;

	switch (dimension) {
	case 1: data.loc = (vec3){- 0.5 * coord[0],  0., coord[0] * SQRT_3_2};   break;
	case 2: data.loc = (vec3){coord[1] - 0.5 * coord[0], 0., coord[0] * SQRT_3_2};  break;
	case 3: data.loc = (vec3){coord[1] - 0.5 * coord[0], coord[2], coord[0] * SQRT_3_2}; break;
	default:  break;
	}
        graph_set_data(graph, i, &data);
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
static int get_number_directions(int tile, int dimension, int nb_tile)
{
    if (dimension == 1)
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
static int get_id_from_move(int origin, int *direction, int dimension,
                                int nb_tile, graph_t *graph)
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

static struct map_methods methods = {
    .init_graph = &init_graph,
    .get_number_directions = &get_number_directions,
    .get_id_from_move = &get_id_from_move,
};

void map_register(struct map_methods* m)
{
    *m = methods;
}
