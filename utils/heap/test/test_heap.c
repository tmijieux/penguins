#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <utils/heap.h>

/**
 * @file
 */

struct heap {
    int (*compar)(void*, void*);
    int (*id)(void*);
    size_t heap_size;
    void **buf;
    int *position;
    int free_buf;
};

static int compar(void *a, void *b)
{
    return 23;
}

static int id(void *a)
{
    return 7;
}

void test_heap_create(void)
{
    struct heap *h = heap_create(10, NULL, &compar, NULL);
    assert(h != NULL);
    assert(h->compar == &compar);

    assert(h->buf != NULL);
    assert(h->free_buf != 0);

    assert(h->id == NULL);
    assert(h->position == NULL);

    heap_destroy(h);
    h = NULL;
    void **buf = malloc(10 * sizeof(*buf));
    if (buf == NULL) {
	printf("some memory allocation needed for a test failed\n");
	exit(EXIT_FAILURE);
    }
    h = heap_create(10, buf, &compar, &id);
    assert(h != NULL);
    assert(h->compar == &compar);

    assert(h->buf == buf);
    assert(h->free_buf == 0);

    assert(h->id == &id);
    assert(h->position != NULL);
    heap_destroy(h);
    free(buf);
}

int main(int argc, char *argv[])
{
    test_heap_create();
    return EXIT_SUCCESS;
}

