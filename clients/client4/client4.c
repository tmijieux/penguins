/**
 * @file
 */

#include <stdio.h>

#include <interface/client.h>
#include <interface/server.h>
#include <interface/move.h>
#include <interface/tile.h>
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
    
static void penguin_remove(struct penguin *p)
{
    p->tile = -1;
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
    int tile = 0;
    int ok = 0;
    
    while (!ok) { 
	tile = display_player_chose_tile();
	if (graph_get_fish(client.graph, tile) != 1) {
	    display_blink_wrong_tile(tile);
	} else {
	    display_blink_good_tile(tile);
	    ok = 1;
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


static int target_is_reachable(int src, int trg, int *dir, int *jmp)
{
    int max_dir = nb_direction(src), ok = 0;
    for (int i = 0; !ok && i < max_dir; i++) {
	int dst = 1;
	while (dst > 0) {
	    dst = move_is_valid(src, i, j);
	    if (dst == trg) {
		*dir = i;
		*jmp = j;
		ok = 1;
		break;
	    }
	}
    }
    return ok;
}

void client_play(struct move *ret)
{
    int src = -1;
    int target = -1;
    int ok = 0, dir, jmp;
    while (!ok) {
	struct mouseclick mc;
	display_mc_get(&mc);
	if (!mc.validclick)
	    continue;
	int p = graph_get_player(client.graph, mc.id);
	if (p == client.id)
	    src = mc.id;
	else if (p > 0)
	    display_blink_bad_penguin(mc.id);
	else if (mc.t == MC_TILE)
	    target = mc.id;
	if (src >= 0 && target >= 0) {
	    ok = target_is_reachable(src, target, &dir, &jmp);
	    if (!ok)
		display_blink_bad_tile(target);
	}
    }
    move_set(ret, tile, dir, jmp);
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
