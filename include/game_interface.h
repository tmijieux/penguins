#ifndef PENGUINS_GAME_INTERFACE_H
#define PENGUINS_GAME_INTERFACE_H

/**
 * function to be called by client(player) modules
 * to obtain information about current game state
 */

int game__get_current_player_id(void);
int game__player_score(int player);


// return -1 if the move is not valid or
// return the id of the destination tile (if it is valid)
int game__move_is_valid(int tile_id, int direction, int nb_move);

int game__get_nb_penguin_player(int player);

int game__get_nb_player(void);



int tile__nb_direction(int tile_id);

// return the number of fish on the tile (from 1 to 3)
int tile__get_fishes(int tile_id);

// return -1 if there is no player or
// return the id of the player (for 4 players, first id is 0 and last is 3)
int tile__get_player(int tile_id);

// return the number of neighbour :
// for one neighbour, return 1 ...
int tile__get_neighbour_count(int tile_id);

// return a table of the ids of the neighbours
const int *tile__get_neighbour(int tile_id);



struct move;
typedef struct move move_t;

// return the id of the origin tile of the move
int move__get_orig(move_t*);

// return the direction of the move
int move__get_direction(move_t*);

// return the number of jump in the move
int move__get_nb_jump(move_t*);

// send the order to the server
// first parameter is the parameter of client_play
// in theory, only the last order is considerated by the server
void move__set(move_t*, int tile_id_orig, int direction, int nb_move);



#endif // PENGUINS_GAME_INTERFACE_H
