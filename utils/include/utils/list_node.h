#ifndef PENGUINS_NODE_H
#define PENGUINS_NODE_H

struct node;
typedef struct node node_t;

#define NODE_SENTINEL 0x00000001
#define NODE_DATA     0x00000002

node_t *node_create(void *data, int flags);
void node_free(node_t*);

node_t *node_get_next(node_t*);
void node_set_next(node_t*, node_t *next);

node_t *node_get_previous(node_t *node);
void node_set_previous(node_t *node, node_t *previous);

void *node_get_data(node_t*);
void node_set_data(node_t*, void *data);

int node_is_sentinel(node_t*);

#endif // PENGUINS_NODE_H
