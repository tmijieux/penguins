#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "utils/vec.h"
#include "utils/graph.h"

#include "coord.h"
#include "penguins/map_interface.h"

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
static void init_graph(int dimension, int nb_tile, struct graph *graph, display_methods_t *display)
{
    int *coord = malloc(dimension * sizeof(int));
    length = coord_get_length_side(nb_tile, dimension);
    int tex = -1;
    int octo_model = -1;
    int square_model = -1;
    if (dimension >= 1 && dimension <= 3) {
        tex = display->register_texture("textures/glace.jpg");
        square_model = display->register_model("models/wavefront/square_tile.obj");
        octo_model = display->register_model("models/wavefront/octogon_tile.obj");
    }
    for (int i = 0; i < nb_tile; i++) {
        int dim = 1;
        coord_get_coordinates_from_id(i, nb_tile, length,
                                      dimension, coord);
        int is_odd = odd(coord[0] + coord[1]);

        for (int j = 0; j < dimension; j++) {
            if (coord[j] + 1 < length && i + dim < nb_tile) {
                graph_add_edge(graph, i, i + dim);
            }
            dim *= length;
        }
        if (is_odd && coord[0] + 1 < length) {
            if (coord[1] + 1 < length && i + length + 1 < nb_tile) {
                graph_add_edge(graph, i, i + length + 1);
            }
            if (coord[1] - 1 >= 0 && i - length + 1 >= 0) {
                graph_add_edge(graph, i, i - length + 1);
            }
        }


        vdata_t data;
        graph_get_data(graph, i, &data);
        data.model_id = is_odd ? octo_model : square_model;
        data.texture_id = tex;
        data.angle = 0;
        data.scale = is_odd ? 0.4 : 0.25;

        switch (dimension) {
        case 1: data.loc = (vec3) {0., 0., coord[0]};             break;
        case 2: data.loc = (vec3) {coord[1], 0., coord[0]};       break;
        case 3: data.loc = (vec3) {coord[1], coord[2], coord[0]}; break;
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
    if (dimension == 1) {
        return 2;
    }

    int *coord = calloc(dimension, sizeof(int));
    coord_get_coordinates_from_id(tile, nb_tile, length, dimension, coord);
    int is_odd = odd(coord[0] + coord[1]);
    free(coord);
    return dimension * 2 + 4 * is_odd;
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
    int *coord = malloc(dimension * sizeof(*coord));
    coord_get_coordinates_from_id(origin, nb_tile, length, dimension, coord);
    int is_odd = odd(coord[0] + coord[1]);
    int dest;

    if (*direction < dimension * 2) {
        coord[*direction / 2] += 1 - 2 * (*direction % 2);
        dest = coord_get_id_from_coordinates(coord, length, dimension);
    } else if (is_odd && *direction < (dimension * 2 + 4)) { // diagonal direction
        int diag_direction = *direction - 2 * dimension;
        coord[0] += 1 - 2 * (diag_direction % 2);
        coord[1] += 1 - 2 * (diag_direction < 2);
        dest = coord_get_id_from_coordinates(coord, length, dimension);
    } else {
        dest = -1;
    }

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
