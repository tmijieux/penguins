#include <stdio.h>
#include <limits.h>
#include <math.h>

#include "penguins/client_interface.h"
#include "penguins/game_interface.h"

#include "utils/graph.h"
#include "utils/list.h"

typedef struct _client {
    int id;
    int nb_tile;
    struct graph *graph;
    struct list *my_penguins;
    int last_penguin;
    int nb_players;
} Client;
static Client client;
static game_methods_t GameM;

struct penguin
{
    int tile;
};


struct penguin *penguin_create(int tile)
{
    struct penguin *p = malloc(sizeof(*p));
    p->tile = tile;
    return p;
}


static void penguin_set_tile(struct penguin *p, int tile)
{
    p->tile = tile;
}

static int penguin_get_tile(struct penguin *p)
{
    return p->tile;
}

static int get_penguin_tile(int player, int penguin, struct graph * graph)
{
    for (int tile = 0; tile < client.nb_tile; ++tile)
    {
        if (graph_get_player_id(graph, tile) == player)
        {
            if (penguin == 0) {
                return tile;
            } else {
                penguin--;
            }
        }
    }
    return -1;
}

static void parse_tile(int tile)
{
    int nc = GameM.get_tile_neighbours_count(tile);
    const int *neighbour = GameM.get_tile_neighbours(tile);
    int fish = GameM.get_tile_nb_fishes(tile);
    graph_set_nb_fish(client.graph, tile, fish);
    for (int i = 0; i < nc; i++)
        graph_add_edge(client.graph, tile, neighbour[i]);
}

static void client_init(int nb_tile, game_methods_t* game_methods)
{
    GameM = *game_methods;
    FILE *fichier = fopen("Log.txt", "w+");
    fclose(fichier);
    client.id = GameM.get_current_player_id();
    client.graph = graph_create(nb_tile, 0, GRAPH_LIST);
    client.my_penguins = list_create(LIST_FREE_MALLOCD_ELEMENT__);
    client.nb_tile = nb_tile;
    client.last_penguin = 0;
    client.nb_players = GameM.get_nb_player();
    for (int i = 0; i < nb_tile; i++) {
        parse_tile(i);
    }
}

static int client_place_penguin(void)
{
    int tile;
    for (tile = 0; graph_get_nb_fish(client.graph, tile) != 1; tile ++);
    struct penguin *penguin = penguin_create(tile);
    list_add_element(client.my_penguins, penguin);
    return tile;
}

static struct penguin *chose_penguin(void)
{
    int l = list_size(client.my_penguins);
    int alea = rand() % l + 1;
    return list_get_element(client.my_penguins, alea);
}

static int do_move(int tile, int dir, int jump, struct graph *graph,
                   int player, int *tile_value)
{
    int dest = GameM.move_is_valid(tile, dir, jump);
    if (dest < 0)
    {
        return -1;
    }
    *tile_value = graph_get_nb_fish(graph, tile);
    graph_set_nb_fish(graph, tile, -1);
    graph_set_player_id(graph, dest, player);
    return dest;
}

static void client_play_safe(struct move *ret)
{
    int dest = -1;
    int tile, max_dir, max_jump;
    struct penguin *p;
    do
    {
        p = chose_penguin();
        tile = penguin_get_tile(p);
        max_dir = max_jump = -1;
        int nb_dir = GameM.get_tile_nb_direction(tile);
        int max_fish = -1;
        for (int dir = 0; dir < nb_dir; dir++)
        {
            for (int jump = 1; jump < 10; jump++)
            {
                if ((dest = GameM.move_is_valid(tile, dir, jump)) > -1)
                {
                    int nb_fish = graph_get_nb_fish(client.graph, dest);
                    if (max_fish < nb_fish)
                    {
                        max_fish = nb_fish;
                        max_dir = dir;
                        max_jump = jump;
                        GameM.set_move(ret, tile, max_dir, max_jump);
                    }
                }
                else
                {
                    break;
                }
            }
        }
    }
    while ((dest = GameM.move_is_valid(tile, max_dir, max_jump)) < 0);
    graph_set_nb_fish(client.graph, tile, -1);
    penguin_set_tile(p, dest);
}

static void undo_move(int tile, int dest, struct graph *graph, int player,
                      int tile_value)
{
    graph_set_nb_fish(graph, tile, tile_value);
    graph_set_player_id(graph, tile, player);
    graph_set_player_id(graph, dest, -1);
}

static void update_tile(int tile)
{
    // an updated tile is necessary for the worst:
    graph_set_nb_fish(client.graph, tile, -1);
    graph_set_player_id(client.graph, tile, GameM.get_tile_player(tile));
}

static void prev_recursif(int tours,
                          long *score_adversaire,
                          struct graph *graph,
                          int player, long *sc)
{
    int player_count = GameM.get_nb_player();
    int player_value = player % player_count;
    int nb_penguin = GameM.get_nb_penguin_player(player_value);

    for (int peng = 0; peng < nb_penguin; ++peng)
    {
        int tile = get_penguin_tile(player_value, peng, graph);
        int nb_dir = GameM.get_tile_nb_direction(tile);
        for (int dir = 0; dir < nb_dir; dir++)
        {
            for (int jump = 1; jump < 10; jump++)
            {
                int tile_value = 0;
                int dest;
                if ((dest = do_move(tile, dir, jump, graph,
                                    player, &tile_value)) < 0)
                {
                    int score = 0;
                    for (int i = 0; i < player_count; i++)
                    {
                        if (i != client.id)
                        {
                            long temp = score_adversaire[i];
                            if (temp > 0) {
                                score += temp;
                            }
                        }
                    }
                    if (score > *sc)
                    {
                        *sc = score;
                    }
                    break;
                }
                score_adversaire[player_value] += graph_get_nb_fish(graph, dest);
                if (tours != 1 && player_value != client.id - 1)
                {
                    if (player_value == client.id) {
                        prev_recursif(tours - 1, score_adversaire,
                                      graph, player_value + 1, sc);
                    } else {
                        prev_recursif(tours, score_adversaire,
                                      graph, player_value + 1, sc);
                    }
                }
                else
                {
                    if (client.id != player_value)
                    {
                        int score = 0;
                        for (int i = 0; i < player_count; i++)
                        {
                            if (i != client.id)
                            {
                                long temp = score_adversaire[i];
                                if (temp > 0) {
                                    score += temp;
                                }
                            }
                        }
                        if (score > *sc) {
                            *sc = score;
                        }
                    }
                }
                undo_move(tile, dest, graph, player, tile_value);
                score_adversaire[player_value] -= graph_get_nb_fish(graph, dest);
            }
        }
    }
}

static void client_play(struct move *ret)
{
    int min_adv = INT_MAX;
    int nb_penguin = list_size(client.my_penguins);
    struct penguin *p = NULL;
    int tile = 0;
    int dest = 1;
    for (int penguin = 1; penguin <= nb_penguin; penguin++)
    {
        struct penguin *peng_temp = list_get_element(client.my_penguins,
                                                     penguin);
        int tile_temp = penguin_get_tile(peng_temp);
        int nb_dir = GameM.get_tile_nb_direction(tile);
        for (int dir = 0; dir < nb_dir; dir++)
        {
            for (int jump = 1; jump < 30; jump++)
            {
                if (GameM.move_is_valid(tile_temp, dir, jump) < 0)
                {
                    break;
                }
                struct graph *copy_graph = graph_copy(client.graph);
                int tile_value;
                do_move(tile_temp, dir, jump, copy_graph, client.id,
                        &tile_value);
                long *t = calloc(client.nb_players, sizeof(*t));
                long sc = 0;
                prev_recursif(10, t, copy_graph, client.id + 1, &sc);
                if (sc < min_adv)
                {
                    min_adv = sc;
                    GameM.set_move(ret, tile_temp, dir, jump);
                    p = peng_temp;
                    tile = tile_temp;
                    dest = GameM.move_is_valid(tile_temp, dir, jump);
                }
                graph_destroy(copy_graph);
            }
        }
    }

    if (min_adv == INT_MAX)
    {
        client_play_safe(ret);
    }
    else
    {
        graph_set_nb_fish(client.graph, tile, -1);
        penguin_set_tile(p, dest);
    }
}


static void send_diff(enum diff_type dt, int orig, int dest)
{
    if (dt == MOVE)
        update_tile(dest);
    update_tile(orig);
}


static void memory_free(void)
{
    if (client.graph != NULL)
        graph_destroy(client.graph);
    if (client.my_penguins != NULL)
        list_destroy(client.my_penguins);
}

static int get_score_from_move(int tile, int dir, int jump,
                               struct graph *graph, int player)
{
    int dest;
    if ((dest = game__move_is_valid(tile, dir, jump)) < 0)
        return -1;
    int score = 0;
    score += graph_get_nb_fish(graph, dest);
    return score;
}

static struct client_methods methods = {
    .init = &client_init,
    .place_penguin = &client_place_penguin,
    .play = &client_play,
    .send_diff = &send_diff,
    .exit = &memory_free,
    .name = "Minimax-Like Penguinz!",
};

void client_register(struct client_methods *cb)
{
    *cb = methods;
}
