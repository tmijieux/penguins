#ifndef TILE_H
#define TILE_H

// return the number of fish on the tile (from 1 to 3)
int tile__get_fishes(int tile_id);

// return -1 if there is no player or
// return the id of the player (for 4 players, first id is 0 and last is 3)
int tile__get_player(int tile_id);

// return the number of neighbour :
// for one neighbour, return 1 ...
int tile__get_neighbour_count(int tile_id);

// return a table of the ids of the neighbours
int *tile__get_neighbour(int tile_id);

#endif //TILE_H
