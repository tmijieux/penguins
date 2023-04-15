#ifndef PENGUINS_LIST_H
#define PENGUINS_LIST_H

#define LIST_DEFAULT__                (0x00000000)
#define LIST_FREE_MALLOCD_ELEMENT__   (0x00000001)

struct list;
typedef struct list list_t;

list_t *list_create(int flags);
void list_destroy(list_t*);

void *list_get_element(list_t*, unsigned int);
void list_add_element(list_t *, void*);

void list_insert_element(list_t*, unsigned int, void*);
void list_remove_element(list_t*, unsigned int);

size_t list_size(list_t*);

#endif // PENGUINS_LIST_H
