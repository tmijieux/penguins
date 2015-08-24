#ifndef MAP_H
#define MAP_H

#include <move.h>
#include <utils/graph.h>

void map_init(const char *map_path);
void map_exit(void);

// Initialise la map en fonction du nombre de dimensions et du
// nombre de sommets, en ajoutant les arêtes à graph.
extern void (*map_init_graph)(int number_dimensions,
			      int number_tiles,
			      struct graph *graph);

// Retourne l'id de la case d'arrivée dest après un mouvement depuis 
// la case origin dans une *direction donnée, ou -1 si le mouvement est invalide.
// Le mappeur peut se permettre de changer la valeur de *direction pour indiquer
// au serveur quelle est la prochaine direction à prendre pour avoir 
// un déplacement en ligne droite sur plusieurs cases
extern int (*map_get_id_from_move)(int origin,
				   int *direction,
				   int number_dimensions,
				   int number_tiles,
				   struct graph *graph);

// Retourne le nombre de directions de la tile
extern int (*map_get_number_directions)(int tile,
					int number_dimensions,
					int nb_tile);

const char *map_get_random_map_name(void);

#endif //MAP_H
