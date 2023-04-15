#ifndef PENGUINS_MAP_INTERFACE_H
#define PENGUINS_MAP_INTERFACE_H

struct graph;

struct map_methods {

    /**
     * Initialisation du graphe.
     * @param int - Dimension du jeu : 1D, 2D, 3D, ...
     * @param int - Nombre de tuiles.
     * @param struct graph * - Graphe du jeu.
     */
    void (*init_graph)(int n_dim, int n_tile, struct graph*);

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
     * @param struct graph * - Graphe du jeu.
     * @return int - Destination du mouvment
     */
    int (*get_id_from_move)(int src, int *direction, int n_dim, int n_tile, struct graph*);

    char dummy1[20];

    void (*exit)(void);
    char dummy2[22];
};

void map_register(struct map_methods*);

#endif // PENGUINS_MAP_INTERFACE_H
