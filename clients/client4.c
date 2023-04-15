#include <stdio.h>

#include "client_interface.h"
#include "game_interface.h"

#include "utils/graph.h"
#include "utils/list.h"
#include "display/mouseclick.h"

struct _client {
    int id;
    graph_t *graph;
    list_t *my_penguins;
    list_t *tile_fish1;
};
static struct _client client;

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
    const int *neighbour = tile__get_neighbour(tile);
    int nb_fish = tile__get_fishes(tile);
    if (nb_fish == 1){
	union iv f = {.i=tile};
	list_add_element(client.tile_fish1, f.v);
    }
    graph_set_nb_fish(client.graph, tile, nb_fish);
    for (int i = 0; i < nc; i++) {
	graph_add_edge(client.graph, tile, neighbour[i]);
    }
}

static void client_init(int nb_tile)
{
    client.id = game__get_current_player_id();
    client.graph = graph_create(nb_tile, 0, GRAPH_LIST);
    client.my_penguins = list_create(LIST_FREE_MALLOCD_ELEMENT__);
    client.tile_fish1 = list_create(LIST_DEFAULT__);
    for (int i = 0; i < nb_tile; i++) {
	parse_tile(i);
    }

    display_mc_init(NULL, 0., 0., 0.);
}

static int client_place_penguin(void)
{
    puts("CLIENT PLACE: user mode start:");
    puts("Click on a empty tile to place your penguin!!");

    int tile_id = -1;
    int ok = 0;

    while (!ok) {
	struct mouseclick mc;
	int ret = display_mc_get(&mc);
	if (ret == TC_DISPLAY_THREAD_STOP ||  ret == TC_SURRENDER) {
	    tile_id = -1;
            break;
	}
        if (!mc.valid_click) {
            continue;
        }
	tile_id = mc.tile_id;
        int nb_fish = graph_get_nb_fish(client.graph, tile_id);
        int player_id = graph_get_player_id(client.graph, tile_id);
        printf("Detected tile by click id=%d  nb_fish=%d player=%d\n",
               tile_id, nb_fish, player_id);

        if (player_id != -1) {
            puts("There is already a penguin on this tile.");
            continue;
        }

    	if (nb_fish != 1) {
    	    //display_blink(BLINK_WRONG, tile);
	    puts("WRONG TILE: must have exactly 1 fish on it");
    	} else {
    	    //display_blink(BLINK_GOOD, tile);
            struct penguin *penguin = penguin_create(tile_id);
            list_add_element(client.my_penguins, penguin);
            break;
    	}
    }

    printf("You have selected the tile %d!\n", tile_id);
    return tile_id;
}

static void update_tile(int tile)
{
    int fish = graph_get_nb_fish(client.graph, tile);
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
    graph_set_nb_fish(client.graph, tile, -1);

    // set the owner of the tile, it may interest us
    graph_set_player_id(client.graph, tile, tile__get_player(tile));
}

static int target_is_reachable(int src, int trg, int *dir, int *jmp)
{
    int max_dir = tile__nb_direction(src);
    int ok = 0;
    for (int i = 0; !ok && i < max_dir; i++) {
	int dst = 1;
	int j = 1;
	while (dst > 0) {
	    dst = game__move_is_valid(src, i, j);
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

static void client_play(struct move *retmov)
{
    puts("CLIENT PLAY: user mode start:");
    puts("You must click on a penguin and on a target tile to move your penguin!");

    int src = -1;
    int target = -1;
    int ok = 0, dir, jmp;
    while (!ok) {
	struct mouseclick mc;
	int err = display_mc_get(&mc);
	if (err == TC_DISPLAY_THREAD_STOP || err == TC_SURRENDER) {
	    move__set(retmov, -1, -1, -1);
	    return;
	}

	if (!mc.valid_click) {
	    continue;
        }
        printf("Detected tile by click = %d\n", mc.tile_id);

	int p = graph_get_player_id(client.graph, mc.tile_id);
	if (p == client.id) {
	    src = mc.tile_id;
	    printf("Source set on tile %d\n", mc.tile_id);
	}
	else if (p > -1) {
	    //display_blink(BLINK_WRONG, mc.tile_id);
	    puts("The penguin on this tile does not belong to you!");
        }
	else if (mc.object_type == MC_TILE) {
	    target = mc.tile_id;
	    printf("Target set on tile %d\n", mc.tile_id);
	}
	if (src >= 0 && target >= 0) {
	    ok = target_is_reachable(src, target, &dir, &jmp);
	    if (!ok) {
                src = -1;
                target = -1;
		//	display_blink(BLINK_WRONG, target);
		puts("Oh! Sadly, we can't reach this delicious-looking fishes from here!");
                puts("Try again with another tile or another penguin!");
            }
	}
    }
    move__set(retmov, src, dir, jmp);
    puts("CLIENT PLAY: user mode end!");
}

static void send_diff(enum diff_type dt, int orig, int dest)
{
    if (dt == MOVE)
	update_tile(dest);
    update_tile(orig);
}

static void memory_free(void)
{
    if (client.graph != NULL) {
	graph_destroy(client.graph);
    }
    if (client.my_penguins != NULL) {
	list_destroy(client.my_penguins);
    }
}

struct client_methods methods = {
    .init = &client_init,
    .place_penguin = &client_place_penguin,
    .play = &client_play,
    .send_diff = &send_diff,
    .exit = &memory_free,
    .name = "Human player: GL click interaction"
};

void client_register(struct client_methods *cb)
{
    *cb = methods;
}
