/**
 * @file tile.c
 */

#include <math.h>

#include "penguins/game_interface.h"

#include "utils/graph.h"
#include "server/server.h"


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
    return graph_get_nb_fish(Game.tile_graph, tile_id);
}

/**
 * Obtenir le joueur présent sur une tuile.
 * @param tile_id - Identidiant de la tuile.
 * @return int - Identifiant de joueur présent dessus la tuile.
 *               -1 S'il n'y a pas de joueur dessus la tuile.
 */
int tile__get_player(int tile_id)
{
    return graph_get_player_id(Game.tile_graph, tile_id);
}

/**
 * Obtenir le nombre de tuiles voisines d'une tuile.
 * @param tile_id - Identidiant de la tuile.
 * @return int - Nombre de tuiles voisines
 */
int tile__get_neighbour_count(int tile_id)
{
    return graph_edge_count(Game.tile_graph, tile_id);
}

/**
 * Obtenir les tuiles voisines d'une tuile.
 * @param tile_id - Identidiant de la tuile.
 * @return int * - Tableau des tuiles voisines.
 */
const int *tile__get_neighbour(int tile_id)
{
    return graph_get_neighbours(Game.tile_graph, tile_id);
}

/*-------- INTERFACE END ----------------*/
/*---------------------------------------*/
