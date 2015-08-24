#ifndef DISPLAY_IFACE_H
#define DISPLAY_IFACE_H

#include <GL/freeglut.h>
#include <GL/glu.h>

#include <d3v/texture.h>
#include <d3v/model.h>

#define DISPLAY_THREAD_RETVAL ((void*)-42)

void display_init(int nb_tile, int nb_penguin);
void display_start(void);
int display_exit(void);

int display_add_tile(int id, struct model *m, struct texture *t,
		     double posx, double posy, double posz,
		     int angle, float scale, int fish_count);

int display_add_penguin(int tile, int player);
int display_add_move(int src, int dst);

void display_add_link(int src, int dst);

void display_register_texture(struct texture *t);
void display_register_model(struct model *m);

#endif	//DISPLAY_IFACE_H
