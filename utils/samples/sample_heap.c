#include <stdio.h>
#include <stdlib.h>

#include <utils/heap.h>
#include <utils/list.h>

#define HEAP_BUFFER_SIZE 50

union iv {
    int i;
    void *v;
};

static void ilist_add_element(struct list *l, int i)
{
    union iv u = { .i = i };
    list_add_element(l, u.v);
}

static int ilist_get_element(struct list *l, int p)
{
    union iv u;
    u.v  = list_get_element(l, p);
    return u.i;
}

static int cmp_list(void *a, void *b)
{
    return list_size(a) - list_size(b);
}

static int id_list(void *a)
{
    return ilist_get_element(a, 1);
}

int main(int argc, char *argv[])
{
    struct heap *heap = heap_create(HEAP_BUFFER_SIZE, NULL,
				    &cmp_list, &id_list);
    struct list *l[3];
    for (int i = 0; i < 3; i++)
	l[i] = list_create(LIST_DEFAULT__);

    ilist_add_element(l[0], 5);
    ilist_add_element(l[0], 7);
    ilist_add_element(l[0], 0);  // --> (0, 5, 7) 3

    ilist_add_element(l[1], 5);
    ilist_add_element(l[1], 1);   // --> (1, 5) 2

    ilist_add_element(l[2], 6);
    ilist_add_element(l[2], 6);
    ilist_add_element(l[2], 6);
    ilist_add_element(l[2], 2);    // --> (2, 6, 6, 6) 4

    heap_insert(heap, l[0]);
    heap_insert(heap, l[1]);
    heap_insert(heap, l[2]);

    list_remove_element(l[2], 2);
    list_remove_element(l[2], 2);
    list_remove_element(l[2], 2);  // --> (2)

    heap_update(heap, l[2]); // magic function
    while (heap_size(heap) > 0) {
	printf("%d\n", ilist_get_element(heap_extract_max(heap), 1));
    }
    
    heap_destroy(heap);
    list_destroy(l[0]);
    list_destroy(l[1]);
    list_destroy(l[2]);
    return 0;
}
