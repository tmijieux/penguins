/**
 * @file server.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>

#include <server/server.h>
#include <server/player.h>
#include <server/tile.h>
#include <server/move.h>
#include <server/client.h>
#include <server/map.h>

#include <utils/graph.h>
#include <utils/log.h>

#include <display.h>

#ifndef TEST
#define STATIC static
#define SERVER
#else
#define STATIC
#define SERVER server
#endif

/**
 * Description du serveur.
 */
STATIC struct SERVER {
    int tile_count;
    struct graph *graph;
    int current_player;
    int distributed_penguins;
    int dimension;
    int place_phase;
} server;

/**
 * Fonction auxiliaire de move_is_valid.
 * @param tile - Origine du mouvement.
 * @param direction - Direction du mouvement.
 * @param nb_move - Nombre de sauts du mouvement.
 * @return int - Identifiant de la destination si valide, -1 sinon.
 */
int move_is_valid_aux(int tile, int direction, int nb_move)
{
    // arguments valides
    if (tile < 0 || tile > server.tile_count
	|| nb_move >= server.tile_count)
	return -1;
    if (nb_move < 1 || direction < 0) {
	log_print(NEVER_LOG__, "invalid direction %d or nb_jump %d\n",
		  direction, nb_move);
	return -1;
    }

    int dim = server.dimension;
    int tc = server.tile_count;
    struct graph *g = server.graph;
    int dest = -1;
    while (nb_move--) {
	dest = map_get_id_from_move(tile, &direction, dim, tc, g);
	if (dest != -1 &&
	    graph_has_edge(g, tile, dest) &&
	    graph_get_player(g, dest) < 0 &&
	    graph_get_fish(g, dest) > 0) {
	    tile = dest;
	} else
	    return -1;
    }
    return dest;
}

/* 
 *   BEGIN ( INTERFACE )
 *     --> (dont touch the names)
 *     --> (dont touch the headers)
 */

/**
 * Demander si un mouvement est valide.
 * @param tile - Origine du mouvement.
 * @param direction - Direction du mouvement.
 * @param nb_move - Nombre de sauts du mouvement.
 * @return int - Identifiant de la destination si valide, -1 sinon.
 */
int move_is_valid(int tile, int direction, int nb_move)
{
    // le joueur est bien la ou il prétend etre:
    /*int player = graph_get_player(server.graph, tile);
    if (player != server.current_player) {
    	log_print(DEBUG_LOG__, "Debug: invalid origin %d,"
		  " player %d at line %d in %s\n",
		  tile, server.current_player,
		  __LINE__, __func__);
    	return -1;
    }*/
    return move_is_valid_aux(tile, direction, nb_move);
}

/**
 * Obtenir l'identifiant du joueur courrant.
 * @return int - Identifiant du joueur.
 */
int get_current_player_id(void)
{
    return server.current_player;
}

/**
 * Obtenir le score d'un joueur.
 * @param player - Identifiant du joueur.
 * @return int - Score du joueur.
 */
int player_score(int player)
{
    return player_get_player_score(player);
}

/**
 * Obtenir le nombre de direction d'une tuile.
 * @param tile - Identifiant de la tuile.
 * @return int - Nombre de directions.
 */
int nb_direction(int tile)
{
    return map_get_number_directions(tile, server.dimension, server.tile_count);
}

/**
 * Obtenir le nombre de pingouins d'un joueur.
 * @param player - Identifiant du joueur.
 * @return int - Nombre de pingouins du joueur. 
 */
int nb_penguin_player(int player)
{
    return player_get_penguin_count(player);
}

/**
 * Obtenir le nombre de joueurs.
 * @return int - Nombre de joueurs.
 */
int nb_player(void)
{
    return player_get_player_count();
}

/*
 *   END ( INTERFACE )
 *     --> (I hope you did not touch the names! (better for you you didn't))
 *     --> (If you touched the headers: your head's off !)
 */

/***********************************************/
/****************** MISC ***********************/

/**
 * Obtenir le gagnant.
 * @return int - Identifiant du gagnant.
 */
int server_get_winner(void)
{
    return player_get_winner();
}

/*******************************************************/
/****************** SERVER CORE ** *********************/

/**
 * Création des relations entre les tuiles dans le graphe.
 */
static void server_compute_neighbours(void)
{
    for (int i = 0; i < server.tile_count; i++) {
	size_t s = graph_edge_count(server.graph, i);
	log_print(DEBUG_LOG__, "compute neighbours %d\nedge: %zu\n", i, s);
	int *neighbours = malloc(s * sizeof(*neighbours));
	struct successor_iterator *si =
	    graph_get_successor_iterator(server.graph, i);
	int j = 0;
	for (iterator_begin(si); !iterator_end(si); iterator_next(si)) {
	    neighbours[j] = iterator_value(si);
	    j++;
	}
	successor_iterator_free(si);
	graph_set_neighbours(server.graph, i, neighbours);
	log_print(DEBUG_LOG__, "compute neighbours %d  __ END\n\n", i);
    }
}

#ifdef USE_GL_DISPLAY__
#ifdef TEST
/**
 * Envoyer au module display toutes les directions possibles.
 * @param tile_number - Nombre de tuiles. 
 * @param dimension - Dimension de la partie : 1D, 2D, 3D, ...
 */
void server_display_directions(int tile_number, int dimension)
{
    for (int i = 0; i < tile_number; i++) {
	int nb_dir = map_get_number_directions(i, dimension,server.tile_count);
	for (int direction = 0; direction < nb_dir; ++direction) {
	    int nb_moves = 1;
	    int dest = move_is_valid_aux(i, direction, nb_moves);
	    while (dest != -1){
		display_add_link(i, dest);
		++nb_moves;
		dest = move_is_valid_aux(i, direction, nb_moves);
	    }
	}
    }
}
#endif
#endif

/**
 * Initialisation du serveur.
 * @param tile_number - Nombre de tuiles.
 * @param max_fish - Nombre de poisson maximum sur une tuile.
 * @param dimension - Dimension du jeu : 1D, 2D, 3D, ...
 * @param map_name - Nom de la carte à charger.
 */
void server_init(int tile_number, int max_fish,
		 int dimension, const char *map_name)
{
    log_print(DEBUG_LOG__, "server init __\n");
    server.tile_count = tile_number;
    player_init();
    server.graph = graph_create(tile_number, NOT_ORIENTED, GRAPH_LIST);
    tile_init(server.graph);
    int fish1_count = 0;
    for (int i = 0; i < tile_number; i++) {
	int fish = rand() % max_fish + 1;
	graph_set_fish(server.graph, i, fish);
	if (fish == 1)
	    fish1_count++;
    }
    int nb_penguin = PENGUINS_PER_CLIENT * player_get_player_count();
    if (tile_number < nb_penguin) {
	fprintf(stderr, "Not enough tile: %d penguins\n", nb_penguin);
	exit(EXIT_FAILURE);
    }
    while (fish1_count < nb_penguin) {
	int tile = rand() % tile_number;
	if (graph_get_fish(server.graph, tile) != 1) {
	    graph_set_fish(server.graph, tile, 1);
	    fish1_count++;
	}
    }
    server.dimension = dimension;
    #ifdef USE_GL_DISPLAY__
    if (dimension >= 1 && dimension <= 3) {
	display_init(tile_number, nb_penguin);
    }
    #endif
    map_init(map_name);
    map_init_graph(dimension, tile_number, server.graph);
    log_print(DEBUG_LOG__, "server init __ END\n");
    #ifdef USE_GL_DISPLAY__
    if (dimension >= 1 && dimension <= 3) {
        #ifdef TEST
        server_display_directions(tile_number, dimension);
        #endif
        display_start();
    }
    #endif
}

/**
 * Libération de la mémoire et des modules chargés
 */
void server_exit(void)
{
    for (int i = 0; i < server.tile_count; i++)
	free(graph_get_neighbours(server.graph, i));
    tile_exit();
    #ifdef USE_GL_DISPLAY__
    if (server.dimension >= 1 && server.dimension <= 3)
	display_exit();
    #endif
    map_exit();
    graph_destroy(server.graph);
    player_exit();
    server.graph = NULL;
}

/******************************************************/
/* ***************** GAME LOGIC ***********************/

/**
 * Obtenir l'état du jeu, plus spécifiquement si c'est la fin.
 * @return int - 1 si c'est la fin du jeu, 0 sinon.
 */
int server_game_end(void)
{
    int pl_count = player_get_player_count();
    for (int player = 0; player < pl_count; player++)
	if (player_can_play(player))
	    return 0;
    return 1;
}

/**
 * Permet de savoir si une tuile est isolée.
 * Bloqué par encerclement de pingouins ou par l'océan.
 * @param tile - Identifiant de la tuile à tester.
 * @return int - 1 si isolée, 0 sinon.
 */
static int server_tile_is_isolated(int tile)
{
    int isolated = 1;
    struct successor_iterator *vsi =
	graph_get_successor_iterator(server.graph, tile);
    for (iterator_begin(vsi); !iterator_end(vsi); iterator_next(vsi)) {
	int neighbour = iterator_value(vsi);
	int nb_fish = graph_get_fish(server.graph, neighbour);
	int player = graph_get_player(server.graph, neighbour);
	if (nb_fish > 0 && player < 0) { 
	    isolated = 0;// Si il reste un voisin vide, la tuile n'est
	    break;       // pas bloquée.
	}
    }
    successor_iterator_free(vsi);
    return isolated;
}

/**
 * Permet de savoir si tout les pingouins ont été distribué pendant
 * la phase de distribution.
 * @return int - 1 si distribution finie, 0 sinon.
 */
static int distributed_all_penguins(void)
{
    int pc = player_get_player_count();
    return server.distributed_penguins == (PENGUINS_PER_CLIENT * pc);
}

/**
 * Vérification si une tuile est isolée et prend une décision si un 
 * pingouins est dessus.
 * @param tile - Identifiant de la tuile.
 */
void server_update_tile(int tile)
{
    if (server_tile_is_isolated(tile)) {
	int fish = graph_get_fish(server.graph, tile);
	int player = graph_get_player(server.graph, tile);
	if (player > -1 && fish > 0) {
	    graph_set_fish(server.graph, tile, -1);
	    player_score_add(player, fish);
	    player_remove_penguin(player, server.place_phase);
	}
    }
}

/**
 * Vérifie ce qu'il pourrait arriver autour d'une tuile mise à jour ou
 * supprimée.
 * @param tile - Identifiant de la tuile. 
 */
void server_update_nearby_tiles(int tile)
{
    struct successor_iterator *vsi =
	graph_get_successor_iterator(server.graph, tile);
    for (iterator_begin(vsi); !iterator_end(vsi); iterator_next(vsi))
	server_update_tile(iterator_value(vsi)); // each neighbour is updated
    successor_iterator_free(vsi);
}

/**
 * Demande à tous les clients de s'initialiser. 
 * @param nb_player - Nombre de joueurs.
 */
void server_init_all_clients(int nb_player)
{
    server_compute_neighbours();
    for (int i = 0; i < nb_player; i++){
	server.current_player = i;
	player_init_player(i, server.tile_count);
    }
}

/**
 * Première phase de la partie: Demander aux joueurs de placer les pingouins.
 * @param nb_player - Nombre de joueurs.
 */
void server_place_penguins(int nb_player)
{
    int player_id = 0;
    while (!distributed_all_penguins()) {
	server.distributed_penguins++;
	if (!player_can_play(player_id)) {
	    player_id = (player_id + 1) % nb_player;
	    continue;
	}
	server.current_player = player_id;
	int tile = player_place_penguin(player_id);
	if (tile >= 0 && graph_get_player(server.graph, tile) < 0 &&
	    graph_get_fish(server.graph, tile) == 1) {
	    graph_set_player(server.graph, tile, player_id);
	    log_print(INFO_LOG__, "player %d place penguin "
		      "on tile %d\n", player_id, tile);
            #ifdef USE_GL_DISPLAY__
            if (server.dimension >= 1 && server.dimension <= 3)
		display_add_penguin(tile, player_id);
            #endif
	    for (int i = 0; i < nb_player; i++)
		player_send_diff(PENGUIN_PLACE, i, tile, -1);
	    server_update_tile(tile);
	    server_update_nearby_tiles(tile);
	} else {
	    player_kick_player(player_id);
	    log_print(INFO_LOG__, "player %d kicked for "
		      "invalid penguin place on tile %d\n", player_id, tile);
	}
	player_id = (player_id + 1) % nb_player;
    }
}

/**
 * Seconde phase de la partie: Déroulement de la partie, les joueurs choisissent
 * chacun leur tour un mouvement.
 * @param nb_player - Nombre de joueurs.
 */
void server_game(int nb_player)
{
    int player_id = 0;
    while (!server_game_end()) {
	if (!player_can_play(player_id)) {
	    player_id = (player_id + 1) % nb_player;
	    continue;
	}
	server.current_player = player_id;
	struct move client_move;
	player_play(player_id, &client_move);
	int orig = move__get_orig(&client_move);
	int dest = move_is_valid(orig,
				 move__get_direction(&client_move),
				 move__get_nb_jump(&client_move));
	if (dest < 0) {
	    player_kick_player(player_id);
	    player_score_reset(player_id);
	    log_print(INFO_LOG__, "player %d kicked for invalid move\n",
		      player_id);
	} else {
	    // handle fish
	    int fish = graph_get_fish(server.graph, orig);
	    player_score_add(player_id, fish);
	    graph_set_fish(server.graph, orig, -1);
	    // move the player
	    graph_set_player(server.graph, orig, -1);
	    graph_set_player(server.graph, dest, player_id);
	    #ifdef USE_GL_DISPLAY__
            if (server.dimension >= 1 && server.dimension <= 3)
	        display_add_move(orig, dest);
	    #endif
	    log_print(INFO_LOG__, "player %d captured tile %d: +%d\n",
		      player_id, dest, fish);
	    // inform all the player of the move
	    for (int i = 0; i < nb_player; i++)
		player_send_diff(MOVE, i, orig, dest);
	    server_update_tile(dest);
	    server_update_nearby_tiles(orig);
	    server_update_nearby_tiles(dest);
	}
	player_id = (player_id + 1) % nb_player;
    }
}

/**
 * Principal fonction du serveur.
 * Contrôle les differentes phases de la partie.
 */
void server_run(void)
{
    int nb_player = player_get_player_count();
    server_init_all_clients(nb_player);
    server.place_phase = 1;
    server_place_penguins(nb_player);
    server.place_phase = 0;
    for (int i = 0; i < nb_player; i++) {
	if (!player_can_play(i))
	    continue;
	
        if (player_get_penguin_count(i) <= 0) {
	    player_kick_player(i);
	    log_print(INFO_LOG__, "player %d kicked because "
		      "he/she has no penguins left\n", i);
	}
    }
    server_game(nb_player);
}
