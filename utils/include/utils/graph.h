#ifndef PENGUINS_GRAPH_H
#define PENGUINS_GRAPH_H

#include <stdlib.h>

#define GRAPH_MATRIX 0
#define GRAPH_LIST   1

#define NOT_ORIENTED    0
#define ORIENTED        1

struct graph;
struct successor_iterator;

/* graph general */
struct graph* graph_create(int vertices_count,
			   int is_oriented, // ORIENTED || NOT_ORIENTED
			   int implementation); // GRAPH_LIST || GRAPH_MATRIX
struct graph *graph_copy(const struct graph*);
void graph_destroy(struct graph*);
int graph_is_oriented(const struct graph*);
size_t graph_size(const struct graph*);
void graph_add_edge(struct graph*, int v_src, int v_dest);
void graph_remove_edge(struct graph*, int v_src, int v_dest);
int graph_has_edge(const struct graph*, int v_src, int v_dest);
int graph_edge_count(const struct graph*, int vertex);
void graph_transpose(struct graph*);

/* successor iterator */
struct successor_iterator
*graph_get_successor_iterator(struct graph *g, int vertex);
void iterator_begin(struct successor_iterator *sc);
void iterator_next(struct successor_iterator *sc);
int iterator_end(struct successor_iterator *sc);
int iterator_value(struct successor_iterator *sc);
void successor_iterator_free(struct successor_iterator *sc);

/*  Vertex properties:  */
void graph_set_fish(struct graph *g, int vertex, int fish);
int graph_get_fish(struct graph *g, int vertex);

void graph_set_player(struct graph *g, int vertex, int player);
int graph_get_player(struct graph *g, int vertex);

void graph_set_neighbours(struct graph *g, int vertex, int *neighbours);
int *graph_get_neighbours(struct graph *g, int vertex);


/*
  Incoming, custom vertex properties at runtime
 */
#endif // PENGUINS_GRAPH_H
