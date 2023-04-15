#ifndef PENGUINS_GRAPH_H
#define PENGUINS_GRAPH_H

#include <stdlib.h>

#include "utils/vec.h"

#define GRAPH_MATRIX 0
#define GRAPH_LIST   1

#define NOT_ORIENTED    0
#define ORIENTED        1

struct graph;
struct successor_iterator;
typedef struct graph graph_t;
typedef struct successor_iterator successor_iter_t;

struct vert_data {
    int nb_fish;
    int player_id; // -1 for empty

    // display info
    int model_id; // -1 for uninitialized
    int texture_id; // -1 for uninitialized
    vec3 loc;
    float angle;
    float scale;
};
typedef struct vert_data vdata_t;

/* graph general */
graph_t* graph_create(int vertices_count,
			   int is_oriented, // ORIENTED || NOT_ORIENTED
			   int implementation); // GRAPH_LIST || GRAPH_MATRIX
graph_t *graph_copy(const graph_t*);
void graph_destroy(graph_t*);
int graph_is_oriented(const graph_t*);
size_t graph_size(const graph_t*);
void graph_add_edge(graph_t*, int v_src, int v_dest);
void graph_remove_edge(graph_t*, int v_src, int v_dest);
int graph_has_edge(const graph_t*, int v_src, int v_dest);
int graph_edge_count(const graph_t*, int vertex);
void graph_transpose(graph_t*);

/* successor iterator */
successor_iter_t*
graph_get_successor_iterator(graph_t *g, int vertex);
void iterator_begin(successor_iter_t *sc);
void iterator_next(successor_iter_t *sc);
int iterator_end(successor_iter_t *sc);
int iterator_value(successor_iter_t *sc);
void successor_iterator_free(successor_iter_t *sc);

/*  Vertex properties:  */
void graph_set_data(graph_t *g, int vertex, const vdata_t *data);
void graph_get_data(graph_t *g, int vertex, vdata_t *data);


void graph_set_nb_fish(graph_t *g, int vertex, int);
int graph_get_nb_fish(graph_t *g, int vertex);


void graph_set_player_id(graph_t *g, int vertex, int);
int graph_get_player_id(graph_t *g, int vertex);



void graph_set_neighbours(graph_t *g, int vertex, int *neighbours);
int *graph_get_neighbours(graph_t *g, int vertex);


/*
  Incoming, custom vertex properties at runtime
 */
#endif // PENGUINS_GRAPH_H
