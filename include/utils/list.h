#ifndef LIST_H
#define LIST_H

#define LIST_DEFAULT__                (0x00000000)
#define LIST_FREE_MALLOCD_ELEMENT__   (0x00000001)

struct list;

struct list *list_create(int flags);
void list_destroy(struct list*);

void *list_get_element(struct list*, unsigned int);
void list_add_element(struct list *, void*);

void list_insert_element(struct list*, unsigned int, void*);
void list_remove_element(struct list*, unsigned int);

size_t list_size(struct list*);

#endif //LIST_H
