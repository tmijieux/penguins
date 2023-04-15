#ifndef PENGUINS_SERVER_H
#define PENGUINS_SERVER_H

//#define USE_GL_DISPLAY__
#define GAME_ERROR           -1
#define PENGUINS_PER_CLIENT   2


void server_init(int tile_number, int max_fish,
                int dimensions, const char *map_path);
void server_exit(void);
void server_run();

int server_get_winner(void);
int move_is_valid_aux(int tile, int direction, int nb_move);

struct _game {
    int nb_tile;
    struct graph *tile_graph;

    int current_player_id;
    int nb_distributed_penguins;
    int nb_dimension;
    int is_place_phase;
    int is_using_display;
};
typedef struct _game game_t;
extern game_t Game;



#endif // PENGUINS_SERVER_H
