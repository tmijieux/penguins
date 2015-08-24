/**
 * @file
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>

#include <client.h>
#include <server.h>
#include <move.h>
#include <tile.h>

#include <utils/graph.h>
#include <utils/heap.h>
#include <utils/list.h>

#include "hack.h"

static int my_move_is_valid_of_mine(int a, int b, int c)
{
    puts("\nWoohoo this is my move_is_valid\n"
	 "bye bye server\n");
    pause();
    exit(EXIT_SUCCESS);
    return -1;
}

__attribute__ ((constructor))
static void hello_dear_server_who_is_your_daddy(void)
{
    substitute_function(&move_is_valid, &my_move_is_valid_of_mine);
}

static struct {
    int id;
    struct graph *board_graph;
    struct heap *fish_tile;
    struct list *my_penguins;
} client;

struct penguin {
    int tile;
};

static struct penguin *penguin_create(int tile)
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

static inline void iheap_insert(struct heap *h, int i)
{
    heap_insert(h, *(void**)&i);
}

static int heap_fish_compare(void *a__, void *b__)
{
    int a = *(int*) &a__; int b = *(int*) &b__;
    return graph_get_fish(client.board_graph, a)
	- graph_get_fish(client.board_graph, b) + 1; // DO NOT REMOVE MY +1 !
}

static int heap_fish_id(void *a__)
{
   return *(int*) &a__;
}

static int iheap_extract_max(struct heap *h)
{
    void *a = heap_extract_max(h);
    return *(int*) &a;
}

static void parse_tile(int tile)
{
    int nc = tile__get_neighbour_count(tile);
    int *neighbour = tile__get_neighbour(tile);
    int fish = tile__get_fishes(tile);
    graph_set_fish(client.board_graph, tile, fish);
    iheap_insert(client.fish_tile, tile);
    for (int i = 0; i < nc; i++)
	graph_add_edge(client.board_graph, tile, neighbour[i]);
}

void client_init(int nb_tile)
{
    client.id = get_current_player_id();
    client.board_graph = graph_create(nb_tile, 0, GRAPH_LIST);
    client.my_penguins = list_create(LIST_FREE_MALLOCD_ELEMENT__);
    client.fish_tile = heap_create(nb_tile, NULL, &heap_fish_compare,
				   &heap_fish_id);
    for (int i = 0; i < nb_tile; i++) 
	parse_tile(i);
}

int client_place_penguin(void)
{
    //int tile = iheap_extract_max(client.fish_tile);
    int tile;
    for(tile = 0; graph_get_fish(client.board_graph, tile) != 1; tile ++);
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

static void update_tile(int tile)
{
    // an updated tile is necessary for the worst:
    graph_set_fish(client.board_graph, tile, -1);
    // update its position in the heap so that we don't chose an invalid tile
    heap_update(client.fish_tile, *(void**)&tile);
    // set the owner of the tile, it may interest us
    graph_set_player(client.board_graph, tile, tile__get_player(tile));
}

void client_play(struct move *ret)
{
    int dest = -1;
    int tile, max_dir, max_jump;
    struct penguin *p;
    do {
	p = chose_penguin();
	tile = penguin_get_tile(p);
	max_dir = max_jump = -1;
	int nb_dir = nb_direction(tile);
	int max_fish = -1;
	for (int dir = 0; dir < nb_dir; dir++) {
	    for (int jump = 1; jump < 10; jump++) {
		if ((dest = move_is_valid(tile, dir, jump)) > -1) {
		    int nb_fish = graph_get_fish(client.board_graph, dest);
		    if (max_fish < nb_fish) {
			max_fish = nb_fish;
			move_set(ret, tile, dir, jump);
			max_dir = dir;
			max_jump = jump;
		    } else
			break;
                   // We cannot go further if it is blocked here
		}
	    }
	}
    } while ((dest = move_is_valid(tile, max_dir, max_jump)) < 0);
    graph_set_fish(client.board_graph, tile, -1);
    penguin_set_tile(p, dest);
}

void send_diff(enum diff_type dt, int orig, int dest)
{
    if (dt == MOVE)
	update_tile(dest);
    update_tile(orig);
}

__attribute__ ((destructor))
static void memory_free(void)
{
    if (client.board_graph != NULL)
	graph_destroy(client.board_graph);
    if (client.fish_tile != NULL)
	heap_destroy(client.fish_tile);
    if (client.my_penguins != NULL)
	list_destroy(client.my_penguins);
}
