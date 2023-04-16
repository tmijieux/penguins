#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "penguins/game_interface.h"
#include "server/move_impl.h"
#include "server/server.h"
#include "utils/graph.h"

#define SERVER_NB_TILE 81
#define SERVER_NB_DIMENSION 3

void test_get_orig(void)
{
    struct move m;
    m.orig = 2;
    assert(move__get_orig(&m) == 2);
}

void test_get_direction(void)
{
    struct move m;
    m.direction = 3;
    assert(move__get_direction(&m) == 3);
}

void test_get_nb_jump(void)
{
    struct move m;
    m.jump_count = 4;
    assert(move__get_nb_jump(&m) == 4);
}

void test_set(void)
{
    struct move m;
    move__set(&m,2,3,4);
    assert(m.orig == 2);
    assert(m.direction == 3);
    assert(m.jump_count == 4);
}

int main(int argc, char *argv[])
{
    test_set();
    test_get_orig();
    test_get_direction();
    test_get_nb_jump();
    return EXIT_SUCCESS;
}
