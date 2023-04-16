#ifndef PENGUINS_MAP_INTERFACE_H
#define PENGUINS_MAP_INTERFACE_H


#include "utils/symbol_visibility.h"
#include "utils/graph.h"

typedef struct display_methods {
    int (*register_texture)(const char *texture);
    int (*register_model)(const char *modelpath);
} display_methods_t;

typedef struct map_methods {

    /**
     * Initialisation du graphe.
     * @param int - Dimension du jeu : 1D, 2D, 3D, ...
     * @param int - Nombre de tuiles.
     * @param graph_t * - Graphe du jeu.
     */
    void (*init_graph)(int n_dim, int n_tile, graph_t*, display_methods_t*);

    /**
     * Obtenir le nombre de directions d'une tuile.
     * @param int - Identifiant de la tuile.
     * @param int - Dimension du jeu.
     * @param int - Nombre total de tuiles.
     * @return int - Nombre de directions.
     */
    int (*get_number_directions)(int tile_id, int n_dim, int n_tile);

    /**
     * Obtenir la destination d'un mouvement.
     * @param int - Origine du mouvement (identifiant de la tuile).
     * @param int * - Direction du mouvement.Le mappeur peut se permettre
     * de changer sa valeur pour indiquer au serveur quelle est la prochaine
     * direction à prendre pour avoir  un déplacement en ligne droite.
     * @param int - Dimension du jeu.
     * @param int - Nombre total de tuiles.
     * @param graph_t * - Graphe du jeu.
     * @return int - Destination du mouvment
     */
    int (*get_id_from_move)(int src, int *direction, int n_dim, int n_tile, graph_t*);

    char dummy1[20];

    void (*exit)(void);
    char dummy2[22];
} map_methods_t;


__declspec(dllexport)
void map_register(map_methods_t*);

#endif // PENGUINS_MAP_INTERFACE_H
