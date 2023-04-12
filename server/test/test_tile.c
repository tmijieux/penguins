#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "server/tile.h"

#define NB_TILE 10

extern struct graph *graph;

void test_init()
{
    assert(graph == NULL);
    tile_module_init((void*) 0x98FEBA7);
    assert(graph == (void*) 0x98FEBA7);
    tile_module_exit();
    assert(graph == NULL);
}

int main(int argc, char *argv[])
{
    test_init();
    return EXIT_SUCCESS;
}
