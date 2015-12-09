#ifndef PLAYER_H
#define PLAYER_H

#include <server/move.h>

// FIXME besoin de diff_type de interface/client.h
// mais pas des prototypes: conflits de nom avec
// l'initialisation dumodule client.
enum diff_type {
	MOVE,
	PENGUIN_PLACE
};


void player_init(void);
void player_exit(void);

int player_get_player_count(void);
int player_get_player_score(int player_id);
int player_get_penguin_count(int player_id);
void player_score_add(int player_id, int fish);
void player_score_reset(int player_id);

int player_get_winner(void);
const char *player_get_name(int player);

int player_can_play(int player_id);
void player_remove_penguin(int player_id, int place_phase);
void player_init_player(int p_id, int nb_tile);
int player_place_penguin(int p_id);
void player_play(int p_id, struct move *move);
void player_send_diff(enum diff_type dt, int client_id,
		      int orig, int dest);
void player_kick_player(int player_id);

#endif //PLAYER_H
