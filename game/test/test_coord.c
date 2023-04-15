#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include <server/coord.h>

#define SERVER_NB_TILE 81
#define SERVER_NB_DIMENSION 3
/**
 * @file
 */

void test_get_length_side(void)
{
    int length = coord_get_length_side(SERVER_NB_TILE, SERVER_NB_DIMENSION);
    assert(length == 5);
}

void test_get_coordinates_from_id(void)
{
    int length = coord_get_length_side(SERVER_NB_TILE, SERVER_NB_DIMENSION);
    int *coord = malloc(SERVER_NB_DIMENSION * sizeof(int));

    coord_get_coordinates_from_id(5, SERVER_NB_TILE, length,
				  SERVER_NB_DIMENSION, coord);
    assert(coord[0] == 0);
    assert(coord[1] == 1);
    assert(coord[3] == 0);
    free(coord);
}

void test_get_id_from_coordinates(void)
{
    int length = coord_get_length_side(SERVER_NB_TILE, SERVER_NB_DIMENSION);
    int *coord = malloc(SERVER_NB_DIMENSION * sizeof(int));
    
    coord[0] = 4;
    coord[1] = 3;
    coord[2] = 2;
    
    int id = coord_get_id_from_coordinates(coord, length, SERVER_NB_DIMENSION);

    assert(id == 69); //2 * 5^2 + 3 * 5^1 + 4 * 5^0
    free(coord);
} 

int main(int argc, char *argv[])
{    
    test_get_length_side();
    test_get_coordinates_from_id();
    test_get_id_from_coordinates();
    return EXIT_SUCCESS;
}
