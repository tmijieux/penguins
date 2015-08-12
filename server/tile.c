/**
 * @file tile.c
 */

#include <math.h>

#include <utils/graph.h>
#include <server/server.h>
#include <server/tile.h>

#ifndef TEST
#define STATIC static
#else
#define STATIC
#endif


STATIC struct graph *graph = NULL;

/**
 * Initialisation de la représentation des tuiles.
 * @param g - Graphe du jeu.
 */
void tile_init(struct graph *g)
{
    graph = g;
}

/**
 * Effectuer les traitements nécessaire à la fermeture
 * du jeu.
 */
void tile_exit(void)
{
    graph = NULL;
}
/*----------- INTERFACE ------------------*/
// From here only getters and setters
// nothing more

/**
 * Obtenir le nombre de poissons d'une tuile.
 * @param tile_id - Identidiant de la tuile.
 * @return int - Nombre de poissons.
 */
int tile__get_fishes(int tile_id)
{
    return graph_get_fish(graph, tile_id);
}

/**
 * Obtenir le joueur présent sur une tuile.
 * @param tile_id - Identidiant de la tuile.
 * @return int - Identifiant de joueur présent dessus la tuile.
 *               -1 S'il n'y a pas de joueur dessus la tuile.
 */
int tile__get_player(int tile_id)
{
    return graph_get_player(graph, tile_id);
}

/**
 * Obtenir le nombre de tuiles voisines d'une tuile.
 * @param tile_id - Identidiant de la tuile.
 * @return int - Nombre de tuiles voisines
 */
int tile__get_neighbour_count(int tile_id)
{
    return graph_edge_count(graph, tile_id);
}

/**
 * Obtenir les tuiles voisines d'une tuile.
 * @param tile_id - Identidiant de la tuile.
 * @return int * - Tableau des tuiles voisines.
 */
int *tile__get_neighbour(int tile_id)
{
    return graph_get_neighbours(graph, tile_id);
}

/*-------- INTERFACE END ----------------*/
/*---------------------------------------*/
