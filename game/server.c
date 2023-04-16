#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#ifdef _WIN32
# define sleep(x) Sleep( (int)((x) * 1000))
#else
# include <unistd.h> // for sleep
#endif


#include "penguins/game_interface.h"
#include "server/server.h"
#include "server/player.h"
#include "server/move_impl.h"
#include "server/client.h"
#include "server/map.h"
#include "display/display_internal.h"

#include "utils/graph.h"
#include "utils/log.h"
#include "utils/symbol_visibility.h"

#include "display/display.h"

game_t Game;

/**
 * Fonction auxiliaire de move_is_valid.
 * @param tile - Origine du mouvement.
 * @param direction - Direction du mouvement.
 * @param nb_move - Nombre de sauts du mouvement.
 * @return int - Identifiant de la destination si valide, -1 sinon.
 */
__internal int move_is_valid_aux(int tile, int direction, int nb_move)
{
    // arguments valides
    if (tile < 0
        || tile > Game.nb_tile
    || nb_move >= Game.nb_tile) {
    return -1;
    }
    if (nb_move < 1 || direction < 0) {
    log_print(NEVER_LOG__, "invalid direction %d or nb_jump %d\n",
          direction, nb_move);
    return -1;
    }

    int dim = Game.nb_dimension;
    int tc = Game.nb_tile;
    struct graph *g = Game.tile_graph;
    int dest = -1;
    while (nb_move--) {
    dest = map.get_id_from_move(tile, &direction, dim, tc, g);
    if (dest != -1 &&
        graph_has_edge(g, tile, dest) &&
        graph_get_player_id(g, dest) < 0 &&
        graph_get_nb_fish(g, dest) > 0) {
        tile = dest;
    } else {
        return -1;
        }
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
int game__move_is_valid(int tile, int direction, int nb_move)
{
    // le joueur est bien la ou il prétend etre:
    /*int player = graph_get_player_id(Game.tile_graph, tile);
    if (player != Game.current_player_id) {
        log_print(DEBUG_LOG__, "Debug: invalid origin %d,"
          " player %d at line %d in %s\n",
          tile, Game.current_player_id,
          __LINE__, __func__);
        return -1;
    }*/
    return move_is_valid_aux(tile, direction, nb_move);
}

/**
 * Obtenir l'identifiant du joueur courrant.
 * @return int - Identifiant du joueur.
 */
int game__get_current_player_id(void)
{
    return Game.current_player_id;
}

/**
 * Obtenir le score d'un joueur.
 * @param player - Identifiant du joueur.
 * @return int - Score du joueur.
 */
int game__get_player_score(int player)
{
    return player_get_player_score(player);
}

/**
 * Obtenir le nombre de direction d'une tuile.
 * @param tile - Identifiant de la tuile.
 * @return int - Nombre de directions.
 */
int tile__nb_direction(int tile)
{
    return map.get_number_directions(tile, Game.nb_dimension, Game.nb_tile);
}

/**
 * Obtenir le nombre de pingouins d'un joueur.
 * @param player - Identifiant du joueur.
 * @return int - Nombre de pingouins du joueur.
 */
int game__get_nb_penguin_player(int player)
{
    return player_get_penguin_count(player);
}

/**
 * Obtenir le nombre de joueurs.
 * @return int - Nombre de joueurs.
 */
int game__get_nb_player(void)
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
    for (int i = 0; i < Game.nb_tile; i++) {
        size_t s = graph_edge_count(Game.tile_graph, i);
        log_print(DEBUG_LOG__, "compute neighbours %d\nedge: %zu\n", i, s);
        int *neighbours = malloc(s * sizeof(*neighbours));
        struct successor_iterator *si = graph_get_successor_iterator(Game.tile_graph, i);
        int j = 0;
        for (iterator_begin(si); !iterator_end(si); iterator_next(si)) {
            neighbours[j] = iterator_value(si);
            j++;
        }
        successor_iterator_free(si);
        graph_set_neighbours(Game.tile_graph, i, neighbours);
        log_print(DEBUG_LOG__, "compute neighbours %d  __ END\n\n", i);
    }
}

int assign_fishes(int nb_tile, int max_fish, struct graph *graph)
{
    int fish1_count = 0;
    for (int i = 0; i < nb_tile; i++) {
    int fish = rand() % max_fish + 1;
    graph_set_nb_fish(Game.tile_graph, i, fish);
    if (fish == 1) {
        fish1_count++;
        }
    }

    int nb_penguin = PENGUINS_PER_CLIENT * player_get_player_count();
    if (nb_tile < nb_penguin) {
    fprintf(stderr, "Not enough tile: %d penguins\n", nb_penguin);
    exit(EXIT_FAILURE);
    }
    while (fish1_count < nb_penguin) {
    int tile = rand() % nb_tile;
    if (graph_get_nb_fish(Game.tile_graph, tile) != 1) {
        graph_set_nb_fish(Game.tile_graph, tile, 1);
        fish1_count++;
    }
    }

    return nb_penguin;
}


/**
 * Initialisation du serveur.
 * @param nb_tile - Nombre de tuiles.
 * @param max_fish - Nombre de poisson maximum sur une tuile.
 * @param dimension - Dimension du jeu : 1D, 2D, 3D, ...
 * @param map_module_name - Nom de la carte à charger.
 */
void server_init(
    int nb_tile,
    int max_fish,
    int nb_dimension,
    const char *map_module_name  )
{
    log_print(DEBUG_LOG__, "server init __\n");

    Game.nb_tile = nb_tile;
    Game.nb_dimension = nb_dimension;
    Game.tile_graph = graph_create(nb_tile, NOT_ORIENTED, GRAPH_LIST);
    Game.is_using_display = 0;

    player_module_init();

    int nb_penguin = assign_fishes(nb_tile, max_fish, Game.tile_graph);

    map_module_load(map_module_name);
    printf("initializing map\n");
    display_methods_t dmethods = {
        .register_texture = &display_register_texture,
        .register_model = &display_register_model,
    };
    map.init_graph(nb_dimension, nb_tile, Game.tile_graph, &dmethods);
    printf("map initialized.\n");

    log_print(DEBUG_LOG__, "server init __ END\n");


    #ifdef USE_GL_DISPLAY__
    if (nb_dimension >= 1 && nb_dimension <= 3) {
        printf("starting display thread...\n");
        create_display_thread(nb_dimension, nb_tile, nb_penguin);
        Game.is_using_display = 1;
    }
    #endif
}

/**
 * Libération de la mémoire et des modules chargés
 */
void server_exit(void)
{
    for (int i = 0; i < Game.nb_tile; i++) {
        free(graph_get_neighbours(Game.tile_graph, i));
    }
    #ifdef USE_GL_DISPLAY__
    if (Game.nb_dimension >= 1 && Game.nb_dimension <= 3) {
        join_display_thread();
    }
    #endif
    map_module_unload();
    graph_destroy(Game.tile_graph);
    player_module_exit();
    Game.tile_graph = NULL;
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
    {
        if (player_can_play(player)) {
            return 0;
        }
    }
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
    struct successor_iterator *vsi = graph_get_successor_iterator(Game.tile_graph, tile);
    for (iterator_begin(vsi); !iterator_end(vsi); iterator_next(vsi)) {
        int neighbour = iterator_value(vsi);
        int nb_fish = graph_get_nb_fish(Game.tile_graph, neighbour);
        int player_id = graph_get_player_id(Game.tile_graph, neighbour);
        if (nb_fish > 0 && player_id < 0) {
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
static int has_distributed_all_penguins(void)
{
    int pc = player_get_player_count();
    return Game.nb_distributed_penguins == (PENGUINS_PER_CLIENT * pc);
}

/**
 * Vérification si une tuile est isolée et prend une décision si un
 * pingouins est dessus.
 * @param tile - Identifiant de la tuile.
 */
void server_update_tile(int tile)
{
    if (server_tile_is_isolated(tile)) {
        int fish = graph_get_nb_fish(Game.tile_graph, tile);
        int player_id = graph_get_player_id(Game.tile_graph, tile);
        if (player_id > -1 && fish > 0) {
            graph_set_nb_fish(Game.tile_graph, tile, -1);
            player_score_add(player_id, fish);
            player_remove_penguin(player_id, Game.is_place_phase);
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
    struct successor_iterator *vsi = graph_get_successor_iterator(Game.tile_graph, tile);
    for (iterator_begin(vsi); !iterator_end(vsi); iterator_next(vsi)) {
        server_update_tile(iterator_value(vsi)); // each neighbour is updated
    }
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
        Game.current_player_id = i;
        player_init_player(i, Game.nb_tile);
    }
}

/**
 * Première phase de la partie: Demander aux joueurs de placer les pingouins.
 * @param nb_player - Nombre de joueurs.
 */
void server_place_penguins(int first_player_id, int nb_player)
{
    int player_id = first_player_id;

    while (!has_distributed_all_penguins())
    {
        Game.nb_distributed_penguins++;
        if (!player_can_play(player_id)) {
            player_id = (player_id + 1) % nb_player;
            continue;
        }
        Game.current_player_id = player_id;
        printf("about to call player_place_penguin !! for player=%d\n", player_id);
        int tile_id = player_place_penguin(player_id);
        if (tile_id >= 0
                && graph_get_player_id(Game.tile_graph, tile_id) < 0
                && graph_get_nb_fish(Game.tile_graph, tile_id) == 1)
        {
            graph_set_player_id(Game.tile_graph, tile_id, player_id);
            log_print(INFO_LOG__, "player %d place penguin "
                  "on tile %d\n", player_id, tile_id);
            for (int i = 0; i < nb_player; i++) {
                player_send_diff(PENGUIN_PLACE, i, tile_id, -1);
            }
            server_update_tile(tile_id);
            server_update_nearby_tiles(tile_id);
        
            if (Game.is_using_display) {
                display_add_penguin(tile_id, player_id);
            }
        } else {
            player_kick_player(player_id);
            log_print(INFO_LOG__, "player %d kicked for "
                  "invalid penguin place on tile %d\n", player_id, tile_id);
        }
        player_id = (player_id + 1) % nb_player;
    }
}


/**
 * Seconde phase de la partie: Déroulement de la partie, les joueurs choisissent
 * chacun leur tour un mouvement.
 * @param nb_player - Nombre de joueurs.
 */
void server_game(int first_player_id, int nb_player)
{
    int player_id = first_player_id;
    fprintf(stderr, "Players are moving penguins...\n");

    while (!server_game_end())
    {
        if (!player_can_play(player_id)) {
            player_id = (player_id + 1) % nb_player;
            continue;
        }
        
        fprintf(stderr, "It's player %d turn...\n", player_id);
        
        Game.current_player_id = player_id;
        struct move client_move;
        player_play(player_id, &client_move);
        int orig = move__get_orig(&client_move);
        int dest = move_is_valid_aux(
                orig,
                move__get_direction(&client_move),
                move__get_nb_jump(&client_move)
            );
        if (dest < 0) {
            player_kick_player(player_id);
            player_score_reset(player_id);
            log_print(INFO_LOG__, "player %d kicked for invalid move\n", player_id);
        } else {
            // handle fish
            int nb_fish = graph_get_nb_fish(Game.tile_graph, orig);
            player_score_add(player_id, nb_fish);
            graph_set_nb_fish(Game.tile_graph, orig, -1);
            // move the player
            graph_set_player_id(Game.tile_graph, orig, -1);
            graph_set_player_id(Game.tile_graph, dest, player_id);
        
                if (Game.is_using_display) {
                display_add_move(orig, dest);
                }
        
            log_print(INFO_LOG__, "player %d captured tile %d: +%d\n",
                  player_id, dest, nb_fish);
            // inform all the player of the move
            for (int i = 0; i < nb_player; i++) {
            player_send_diff(MOVE, i, orig, dest);
                }
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
void server_run()
{
    int nb_player = player_get_player_count();
    server_init_all_clients(nb_player);
    if (nb_player == 0) {
        printf("Sadly, no players have joined the game");
        printf("This may be because of a problem.");
        printf("Ensure the player module are loading correctly.");
        return;
    }

    Game.is_place_phase = 1;

    fprintf(stderr, "Players are placing penguins...\n");
    if (Game.is_using_display) {
        printf("waiting for display to be ready\n");

        // TODO replace this with a mutex and condition
        while (!Display.thread_running) {
            sleep(1.0/60.0);
        }
    }
    printf("Display is ready\n");

    int first_player_id = rand() % nb_player;
    printf("first_player_id=%d\n", first_player_id);
    server_place_penguins(first_player_id, nb_player);

    fprintf(stderr, "Players finished placing penguins...\n");
    Game.is_place_phase = 0;

    for (int i = 0; i < nb_player; i++) {
        if (!player_can_play(i)) {
            continue;
        }
        if (player_get_penguin_count(i) <= 0) {
            player_kick_player(i);
            log_print(INFO_LOG__, "player %d kicked because "
                  "he/she has no penguins left\n", i);
        }
    }

    server_game(first_player_id, nb_player);
}
