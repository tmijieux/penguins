/**
 * @file map_triangle.c
 */

#include <stdio.h>
#include <stdlib.h>

#include <server/map.h>
#include <server/coord.h>
#include <utils/log.h>

#include <display.h>
#include <d3v/texture.h>
#include <d3v/model.h>

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
    for (int i = 0; i < size; i++)
	coord[i] += vector[i];
}    

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
	m = model_load_wavefront("models/triangle_tile.obj");
        t = texture_load("textures/glace.jpg");
	display_register_texture(t);
	display_register_model(m);
    }
    #endif

    for (int i = 0; i < nb_tile; i++) {
	int dim = length * length;
	coord_get_coordinates_from_id(i, nb_tile,
				      length, dimension, coord);
	int odd_ = odd(coord[0] + coord[1]);
	
        #ifdef USE_GL_DISPLAY__
	int fish = graph_get_fish(graph, i);
	switch (dimension) {
	case 1:
	    display_add_tile(i, m, t, 
			     -odd_ * SQRT_3 / 3, 0., coord[0], 
			     90. + odd_ * 180., 0.7, fish);   
	    break;
	case 2:
	    display_add_tile(i, m, t,
			     coord[1] * SQRT_3 - odd_ * SQRT_3 / 3,
			     0., 
			     coord[0],
			     90. + odd_ * 180., 0.7, fish);
	    break;
	case 3:
	    display_add_tile(i, m, t,
			     coord[1] * SQRT_3 - odd_ * SQRT_3 / 3, 
			     coord[2], 
			     coord[0],
			     90. + odd_ * 180., 0.7, fish);
	    break;
	default:
	    break;
	}
	#endif	

	if (coord[0] + 1 < length && i + 1 < nb_tile)
	    graph_add_edge(graph, i, i + 1);

	if (!odd_ && coord[1] + 1 < length && i + length < nb_tile)
	    graph_add_edge(graph, i, i + length);
	
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
    if (dimension == 1)
	return 2;
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

