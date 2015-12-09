#include <stdlib.h>
#include <stdio.h>

#include <server/server.h>
#include <server/path.h>

#ifndef SERVER_INSTALL_PATH
#error "test_init_server.c NEED const SERVER_INSTALL_PATH"
#define SERVER_INSTALL_PATH
// this last define is needed: if removed, the compiler pop up an error:
// undefined symbol when it check for the rest of the file
#endif

#define SERVER_NB_TILE 48
#define MAX_FISH 3

/**
 * @file
 */

void test_init_server(void)
{
    server_init(SERVER_NB_TILE, MAX_FISH, 2, "libmap_penrose.so");
    server_exit();
}

int main(int argc, char *argv[])
{
    path_init(argv[0]);
    test_init_server();
    path_exit();
    return EXIT_SUCCESS;
}
