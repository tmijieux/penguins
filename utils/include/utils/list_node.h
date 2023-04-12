#ifndef PENGUINS_NODE_H
#define PENGUINS_NODE_H

struct node;

#define NODE_SENTINEL 0x00000001
#define NODE_DATA     0x00000002

struct node *node_create(void *data, int flags);
void node_free(struct node*);

struct node *node_get_next(struct node*);
void node_set_next(struct node*, struct node *next);

struct node *node_get_previous(struct node *node);
void node_set_previous(struct node *node, struct node *previous);

void *node_get_data(struct node*);
void node_set_data(struct node*, void *data);

int node_is_sentinel(struct node*);

#endif // PENGUINS_NODE_H
