/**
 * @file map_penrose.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "server/map.h"
#include "display/display.h"

#include "./penrose/penrose.h"

/**
 * Initialisation du graphe.
 * @param dimension - Dimension du jeu : 1D, 2D, 3D, ...
 * @param nb_tile - Nombre de tuiles.
 * @param graph - Graphe du jeu.
 */
static void init_graph(int dimension, int nb_tile, struct graph *graph)
{
    if (dimension <= 1) {
	fprintf(stderr, "map_init_graph: Error, map_penrose "
	       "can't be loaded with dimension lower than 2\n");
	exit(EXIT_FAILURE);
    }
    penrose_init(nb_tile, dimension);
    int surface = penrose_get_surface();
    int layer = penrose_get_layer();
    #ifdef USE_GL_DISPLAY__
    float coord[3];
    int tex = -1;
    int acute_model = -1;
    int obtuse_model = -1;
    if (dimension <= 3) {
        tex = display_register_texture("textures/glace.jpg");
        acute_model = display_register_model("models/wavefront/acute_tile.obj");
	obtuse_model = display_register_model("models/wavefront/obtuse_tile.obj");
    }
    #endif
    for (int i = 0; i < nb_tile; i++) {
        int dim = surface;
	int neighbor;
	int j = 0;
        do {
            neighbor = penrose_get_neighbor(i, j);
	    if (neighbor != -1 && !graph_has_edge(graph, i, neighbor))
		graph_add_edge(graph, i, neighbor);
	    j++;
        } while (neighbor != -1 && j < 4);

	for (j = 2; j < dimension; j++) {
	    if (coord[j] + 0.5 < layer && i + dim < nb_tile)
		graph_add_edge(graph, i, i + dim);
	    dim *= layer;
	}


	penrose_get_coordinates_from_id(i, coord);
	float angle = penrose_get_angle(i);
	int type = penrose_get_type(i);
        int model = type != 0 ? acute_model : obtuse_model;

        vdata_t data;
	graph_get_data(graph, i, &data);
        data.model_id = model;
        data.texture_id = tex;
        data.angle = angle;
        data.scale = 0.4;
        data.loc = (vec3){coord[1], coord[2], coord[0]};
        graph_set_data(graph, i, &data);
    }
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
    return dimension * 2;
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
                            int nb_tile, struct graph *graph)
{
    int layer = penrose_get_layer();
    int dim = penrose_get_surface();
    int dest = -1;

    if (*direction < 4) {
	dest = penrose_get_id_from_move(origin, direction);
    } else if (*direction < dimension * 2) {
	for (int i = 0; i < *direction / 2 - 2; i++) {
	    dim *= layer;
	}
	dest = origin + dim * (1 - 2 * (*direction % 2));
    }

    if (dest < 0 || dest >= nb_tile)
	dest = -1;
    return dest;
}

static void my_free(void)
{
    penrose_free();
}

static struct map_methods methods = {
    .init_graph = &init_graph,
    .get_number_directions = &get_number_directions,
    .get_id_from_move = &get_id_from_move,
    .exit = &my_free,
};

void map_register(struct map_methods* m)
{
    *m = methods;
}
