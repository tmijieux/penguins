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
#include <display/mouseclick.h>

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
    
    display_mc_init(NULL, 0., 0., 0.);
}

int client_place_penguin(void)
{
    puts("CLIENT PLACE: user mode start!");
    int tile = 0;
    int ok = 0;
    
    while (!ok) {
	struct mouseclick mc;
	int ret = display_mc_get(&mc);
	if (ret == DISPLAY_THREAD_STOP ||
	    ret == SURRENDER) {
	    return -1;
	}

	tile = mc.tile_id;
    	if (graph_get_fish(client.graph, tile) != 1) {
    	    //display_blink(BLINK_WRONG, tile);
	    puts("WRONG TILE: must have exactly 1 fish on it");
    	} else {
    	    //display_blink(BLINK_GOOD, tile);
    	    ok = 1;
    	}
    }

    struct penguin *penguin = penguin_create(tile);
    list_add_element(client.my_penguins, penguin);

    puts("CLIENT PLACE: user mode end!");
    return tile;
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
	int j = 1;
	while (dst > 0) {
	    dst = move_is_valid(src, i, j);
	    if (dst == trg) {
		*dir = i;
		*jmp = j;
		ok = 1;
		break;
	    }
	    j++;
	}
    }
    return ok;
}

void client_play(struct move *retmov)
{
    puts("CLIENT PLAY: user mode start!");
    int src = -1;
    int target = -1;
    int ok = 0, dir, jmp;
    while (!ok) {
	struct mouseclick mc;
	int ret = display_mc_get(&mc);
	if (ret == DISPLAY_THREAD_STOP ||
	    ret == SURRENDER) {
	    move_set(retmov, -1, -1, -1);
	    return;
	}
	if (!mc.validclick)
	    continue;
	int p = graph_get_player(client.graph, mc.tile_id);
	if (p == client.id) {
	    src = mc.tile_id;
	    printf("Source set on tile %d\n", mc.tile_id);
	}
	else if (p >= 0)
	    //display_blink(BLINK_WRONG, mc.tile_id);
	    puts("WRONG TILE: This is the enemy!! Just Look !!!");
	else if (mc.t == MC_TILE) {
	    target = mc.tile_id;
	    printf("Target set on tile %d\n", mc.tile_id);
	}
	if (src >= 0 && target >= 0) {
	    ok = target_is_reachable(src, target, &dir, &jmp);
	    if (!ok)
		//	display_blink(BLINK_WRONG, target);
		puts("Oh! we can't reach this "
		     "delicious-looking fishes from here!");
	}
    }
    move_set(retmov, src, dir, jmp);
    puts("CLIENT PLAY: user mode end!");
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
