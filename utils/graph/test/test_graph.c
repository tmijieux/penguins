#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include <interface/tile.h>
#include <server/tile.h>
#include <utils/graph.h>
#include <utils/sorted_list.h>

#define NB_TILE 10
#define ORIENTATION NOT_ORIENTED
#define IMPLEMENTS GRAPH_LIST

/**
 * @file
 */

struct successor_iterator {
    int idx;
    struct sorted_list *successors;
};

struct vertex {
    int fish;
    int player;
    int *neighbours;
};

struct graph {
    int is_oriented;
    int vert_count;
    struct vertex *vert;
    struct sorted_list *edges[];
};

void test_graph_orientation(void)
{
    struct graph *g = graph_create(NB_TILE, ORIENTED, IMPLEMENTS);
    graph_add_edge(g, 4, 6);
    assert(graph_has_edge(g, 4, 6));
    assert(!graph_has_edge(g, 6, 4));

    graph_destroy(g);

    g = graph_create(NB_TILE, NOT_ORIENTED, IMPLEMENTS);
    graph_add_edge(g, 4, 6);
    assert(graph_has_edge(g, 4, 6));
    assert(graph_has_edge(g, 6, 4));
    graph_destroy(g);
}

void test_set_fish(void)
{
    struct graph *g = graph_create(NB_TILE, ORIENTATION, IMPLEMENTS);
    graph_set_fish(g, 3, 5);
    assert(g->vert[3].fish == 5);
    graph_destroy(g);
}

void test_get_fish(void)
{
    struct graph *g = graph_create(NB_TILE, ORIENTATION, IMPLEMENTS);
    g->vert[7].fish = 5;
    assert(graph_get_fish(g, 7) == 5);
    graph_destroy(g);
}

void test_set_player(void)
{
    struct graph *g = graph_create(NB_TILE, ORIENTATION, IMPLEMENTS);
    graph_set_player(g, 2, 5);
    assert(g->vert[2].player == 5);
    graph_destroy(g);
}

void test_get_player(void)
{
    struct graph *g = graph_create(NB_TILE, ORIENTATION, IMPLEMENTS);
    g->vert[8].player = 5;
    assert(graph_get_player(g, 8) == 5);
    graph_destroy(g);
}

void test_get_neighbour(void)
{
    struct graph *g = graph_create(NB_TILE, ORIENTATION, IMPLEMENTS);
    int tab[5];
    g->vert[0].neighbours = tab;
    assert(graph_get_neighbours(g, 0) == g->vert[0].neighbours);
    graph_destroy(g);
}

void test_set_neighbour(void)
{
    struct graph *g = graph_create(NB_TILE, ORIENTATION, IMPLEMENTS);
    int tab[5];
    graph_set_neighbours(g, 0, tab);
    assert(g->vert[0].neighbours == tab);
    graph_destroy(g);
}

void test_graph_has_edge(void)
{
    struct graph *g = graph_create(NB_TILE, ORIENTATION, IMPLEMENTS);
    union {
	int i;
	void *v;
    } iv = {.i = 6};
    slist_add_value(g->edges[4], iv.v);
    assert(graph_has_edge(g, 4, 6));
    graph_destroy(g);
}

void test_graph_add_edge(void)
{
    struct graph *g = graph_create(NB_TILE, ORIENTATION, IMPLEMENTS);
    graph_add_edge(g, 5, 8);
    assert(graph_has_edge(g, 5, 8));
    graph_destroy(g);
}

void test_graph_remove_edge(void)
{
    struct graph *g = graph_create(NB_TILE, ORIENTATION, IMPLEMENTS);
    graph_add_edge(g, 2, 7);
    graph_remove_edge(g, 2, 7);
    assert(!graph_has_edge(g, 2, 7));
    graph_destroy(g);
}

int main(int argc, char *argv[])
{
    test_set_fish();
    test_get_fish();
    test_set_player();
    test_get_player();
    test_get_neighbour();
    test_set_neighbour();

    test_graph_has_edge();
    test_graph_add_edge();
    test_graph_remove_edge();

    test_graph_orientation();
    
    return EXIT_SUCCESS;
}
