#ifndef PENGUINS_SORTED_LIST_H
#define PENGUINS_SORTED_LIST_H

#include <utils/list_node.h>

struct sorted_list;
typedef struct sorted_list sorted_list_t;

// flags are useless for now
sorted_list_t *slist_create(int flags, int (*compare)(void*, void*));
void slist_destroy(sorted_list_t*);

size_t slist_size(sorted_list_t*);

// the two following functions can be used to iterate through the list
// thanks to a builtin cursor feature
struct node *slist_get_node(sorted_list_t*, unsigned int pos);
void *slist_get_data(sorted_list_t*, unsigned int pos);

//remove the element is position pos (starting from 1)
void slist_remove(sorted_list_t*, int pos);

// following operations are random access
// (minus the sorted feature) into a list, so
// dont expect performance
void slist_add_value(sorted_list_t*, void *value);
void slist_remove_value(sorted_list_t*, void *value);

// return boolean : value in slist ?
int slist_find_value(sorted_list_t*, void *value);

#endif // PENGUINS_SORTED_LIST_H
