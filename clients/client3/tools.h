#ifndef TOOLS_H
#define TOOLS_H

int get_score_from_move(int tile, int dir, int jump, struct graph *graph,
			int player);

int do_move(int tile, int dir, int jump, struct graph *graph,
	    int player, int *tile_value);

void undo_move(int tile, int dest, struct graph *graph, int player,
	       int tile_value);

void prev_recursif(int tours, long *score_adversaire,
		  struct graph *graph, int player, long *sc);

int get_penguin_tile(int player, int penguin, struct graph * graph);

void client_play_safe(struct move *ret);

#endif
