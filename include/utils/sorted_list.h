#ifndef SORTED_LIST_H
#define SORTED_LIST_H

#include <utils/list_node.h>

struct sorted_list;

// flags are useless for now
struct sorted_list *slist_create(int flags, int (*compare)(void*, void*));
void slist_destroy(struct sorted_list*);

size_t slist_size(struct sorted_list*);

// the two following functions can be used to iterate through the list
// thanks to a builtin cursor feature
struct node *slist_get_node(struct sorted_list*, unsigned int pos);
void *slist_get_data(struct sorted_list*, unsigned int pos);

//remove the element is position pos (starting from 1)
void slist_remove(struct sorted_list*, int pos);

// following operations are random access
// (minus the sorted feature) into a list, so
// dont expect performance
void slist_add_value(struct sorted_list*, void *value);
void slist_remove_value(struct sorted_list*, void *value);

// return boolean : value in slist ?
int slist_find_value(struct sorted_list*, void *value);

#endif //SORTED_LIST_H
