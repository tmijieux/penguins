/**
 * @file
 */

#include <stdio.h>

#include <client.h>
#include <server.h>
#include <move.h>
#include <tile.h>

#include <utils/graph.h>
#include <utils/list.h>

static struct {
    int id;
    struct graph *graph;
    struct list *my_penguins;
    struct list *tile_fish1;
} client;

struct penguin {
    int tile;
};

union iv {
    int i;
    void *v;
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

static void parse_tile(int tile)
{
    int nc = tile__get_neighbour_count(tile);
    int *neighbour = tile__get_neighbour(tile);
    int fish = tile__get_fishes(tile);
    if (fish == 1){
	union iv f = {.i=tile};
	list_add_element(client.tile_fish1, f.v);
    }
    graph_set_fish(client.graph, tile, fish);
    for (int i = 0; i < nc; i++)
	graph_add_edge(client.graph, tile, neighbour[i]);
}

void client_init(int nb_tile)
{
    client.id = get_current_player_id();
    client.graph = graph_create(nb_tile, 0, GRAPH_LIST);
    client.my_penguins = list_create(LIST_FREE_MALLOCD_ELEMENT__);
    client.tile_fish1 = list_create(LIST_DEFAULT__);
    for (int i = 0; i < nb_tile; i++) 
	parse_tile(i);
}

static int greater_fish_count_neighbour(int tile)
{
    struct successor_iterator *it
	= graph_get_successor_iterator(client.graph, tile);
    int max = -2;
    for (iterator_begin(it); !iterator_end(it); iterator_next(it)) {
	int nti = iterator_value(it);
	int fish = graph_get_fish(client.graph, nti);
	if (fish > max) {
	    max = fish;
	}
    }
    return  max;
}

int client_place_penguin(void)
{
    int tile = 0; int max = -2;
    int l = list_size(client.tile_fish1);
    for (int i = 1; i <= l; i++) {
    	union iv ti;
    	ti.v = list_get_element(client.tile_fish1, i);
    	int fish = greater_fish_count_neighbour(ti.i);
    	if (fish > max) {
    	    max = fish;
    	    tile = ti.i;
    	}
    }

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
    int fish = graph_get_fish(client.graph, tile);
    if (fish == 1) {
	int l = list_size(client.tile_fish1);
	for (int i = 1; i <= l; i++) {
	    union iv ti;
	    ti.v = list_get_element(client.tile_fish1, i);
	    if (ti.i == tile) {
		list_remove_element(client.tile_fish1, i);
		break;
	    }
	}
    }
    // an updated tile is necessary for the worst:
    graph_set_fish(client.graph, tile, -1);

    // set the owner of the tile, it may interest us
    graph_set_player(client.graph, tile, tile__get_player(tile));
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
		    int nb_fish = graph_get_fish(client.graph, dest);
		    if (max_fish < nb_fish) {
			max_fish = nb_fish;
			max_dir = dir;
			max_jump = jump;
			move_set(ret, tile, max_dir, max_jump);
		    }
		} else {
		    break;
		}
	    }
	}
    } while ((dest = move_is_valid(tile, max_dir, max_jump)) < 0);
    graph_set_fish(client.graph, tile, -1);
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
    if (client.graph != NULL)
	graph_destroy(client.graph);
    if (client.my_penguins != NULL)
	list_destroy(client.my_penguins);
}
