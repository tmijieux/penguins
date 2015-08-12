#ifndef DISPLAY_H
#define DISPLAY_H

#include <GL/freeglut.h>
#include <GL/glu.h>

#include "camera.h"
#include "light.h"
#include "model.h"
#include "texture.h"
#include "record.h"
#include "animator.h"
#include "dpenguin.h"
#include "dtile.h"

#define DISPLAY_THREAD_RETVAL ((void*)-42)
#define NB_MOVE 30

#define WINDOW_TITLE "Penguin"
#define WINDOW_POSITION_X 200
#define WINDOW_POSITION_Y 200

#define HEIGHT  700
#define WIDTH   700

#define PENGUIN_FILE "models/penguin.obj"

void display_init(int nb_tile, int nb_penguin);
void display_start(void);
void display_exit(void);

int display_add_tile(int id, struct model *m, struct texture *t,
		     double posx, double posy, double posz,
		     int angle, float scale, int fish_count);
int display_add_penguin(int tile, int player);
int display_add_move(int tileSrc, int tileDest);

void display_add_link(int src, int dst);

void display_register_texture(struct texture *t);
void display_register_model(struct model *m);

#endif	//DISPLAY_H
