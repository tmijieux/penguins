#ifndef PENG_SERVER_H
#define PENG_SERVER_H

//#define USE_GL_DISPLAY__
#define GAME_ERROR           -1
#define PENGUINS_PER_CLIENT   2

#include <interface/server.h>

void server_init(int tile_number, int max_fish,
		 int dimensions, const char *map_path);
void server_exit(void);
void server_run(void);

int server_get_winner(void);


#endif //PENG_SERVER_H
