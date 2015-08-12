#ifndef MOVE_H
#define MOVE_H

struct move;

// return the id of the origin tile of the move
int move__get_orig(struct move*);

// return the direction of the move
int move__get_direction(struct move*);

// return the number of jump in the move
int move__get_nb_jump(struct move*);

// send the order to the server
// first parameter is the parameter of client_play
// in theory, only the last order is considerated by the server
void move_set(struct move*, int tile_id_orig,
	      int direction, int nb_move);

#endif //MOVE_H
