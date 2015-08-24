#ifndef SERVER_H
#define SERVER_H

int get_current_player_id(void);
int player_score(int player);
int nb_direction(int tile_id);

// return -1 if the move is not valid or 
// return the id of the destination tile (if it is valid) 
int move_is_valid(int tile_id, int direction, int nb_move);

int nb_penguin_player(int player);

int nb_player(void);

#endif //SERVER_H
