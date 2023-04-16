/**
 * @file map_triangle.c
 */

#include <stdio.h>
#include <stdlib.h>

#include "coord.h"
#include "penguins/map_interface.h"

#define SQRT_3 1.73205080757

static int length;

/**
 * Indique la parité d'un nombre.
 * @param x - Nombre à tester.
 * @return int - 0 Pair, 1 Impair.
 */
static inline int odd(int x)
{
    return x % 2;
}

/**
 * Récupère les vecteurs dans le cas tuile pair et impair
 * pour une direction donnée.
 * @param vector0 - Vecteur pair.
 * @param vector1 - Vecteur impair.
 * @param direction - Direction du déplacement.
 * @param size - Taille des tableau.
 */
static void get_vectors_from_direction(int vector0[], int vector1[],
                                       int direction, int size)
{
    int step;
    switch (direction) {
    case 0:
        vector0[0] = 1;
        vector1[0] = 1;
        break;
    case 1:
        vector0[0] = -1;
        vector1[0] = -1;
        break;
    case 2:
        vector0[1] = 1;
        vector1[0] = 1;
        break;
    case 3:
        vector0[0] = -1;
        vector1[1] = -1;
        break;
    case 4:
        vector0[0] = 1;
        vector1[1] = -1;
        break;
    case 5:
        vector0[1] = 1;
        vector1[0] = -1;
        break;
    default:
        step = 1 - 2 * (direction % 2);
        vector0[direction / 2 -1] = step;
        vector1[direction / 2 -1] = step;
        break;
    }
}

/**
 * Ajoute le vecteur aux coordonnées.
 * @param coord - Coordonnées à modifier.
 * @param vector - Vecteur à ajouter.
 * @param size - Taille des tableaux.
 */
static void add_vector(int coord[], int vector[], int size)
{
    for (int i = 0; i < size; i++) {
        coord[i] += vector[i];
    }
}

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
    int model = -1;
    int texture = -1;
    if (dimension >= 1 && dimension <= 3) {
        model = display->register_model("models/wavefront/triangle_tile.obj");
        texture = display->register_texture("textures/glace.jpg");
    }

    for (int i = 0; i < nb_tile; i++) {
        int dim = length * length;
        coord_get_coordinates_from_id(i, nb_tile, length, dimension, coord);
        int is_odd = odd(coord[0] + coord[1]);

        if (coord[0] + 1 < length && i + 1 < nb_tile) {
            graph_add_edge(graph, i, i + 1);
        }

        if (!is_odd && coord[1] + 1 < length && i + length < nb_tile) {
            graph_add_edge(graph, i, i + length);
        }

        for (int j = 2; j < dimension; j++) {
            if (coord[j] + 1 < length && i + dim < nb_tile) {
                graph_add_edge(graph, i, i + dim);
            }
            dim *= length;
        }


        vdata_t data;
        graph_get_data(graph, i, &data);
        data.model_id = model;
        data.texture_id = texture;
        data.angle = 90. + is_odd * 180.;
        data.scale = 0.7;

        switch (dimension) {
        case 1:
            data.loc = (vec3) {
                is_odd * SQRT_3 / 3,
                0.,
                coord[0]
            };
            break;
        case 2:
            data.loc = (vec3) {
                coord[1] * SQRT_3 - is_odd * SQRT_3 / 3,
                0.,
                coord[0]
            };
            break;
        case 3:
            data.loc = (vec3) {
                coord[1] * SQRT_3 - is_odd * SQRT_3 / 3,
                coord[2],
                coord[0]
            };

            break;
        default:
            break;
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
static int get_number_directions(int tile, int dimension, int nb_tile)
{
    if (dimension == 1) {
        return 2;
    }
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
                            int nb_tile, struct graph *graph)
{
    int *coord = malloc(dimension * sizeof(*coord));
    int *vector0 = calloc(dimension, sizeof(*vector0));
    int *vector1 = calloc(dimension, sizeof(*vector1));

    coord_get_coordinates_from_id(origin, nb_tile, length, dimension, coord);
    get_vectors_from_direction(vector0, vector1, *direction, dimension);

    if (odd(coord[0] + coord[1])) {
        int *tmp = vector0;
        vector0 = vector1;
        vector1 = tmp;
    }
    add_vector(coord, vector0, dimension);

    int dest = coord_get_id_from_coordinates(coord, length, dimension);

    free(coord);
    free(vector0);
    free(vector1);
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
