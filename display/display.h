#ifndef DISPLAY_H
#define DISPLAY_H

#include <display/display.h>

#include "camera.h"
#include "light.h"
#include "model.h"
#include "texture.h"
#include "record.h"
#include "animator.h"
#include "dpenguin.h"
#include "dtile.h"

#define NB_MOVE 30

#define WINDOW_TITLE "Penguin"
#define WINDOW_POSITION_X 200
#define WINDOW_POSITION_Y 200

#define HEIGHT  700
#define WIDTH   700

#define PENGUIN_FILE "models/penguin.obj"


void scene_mouseclick_mode(int v);

struct scene {
    int key;
    // variables de la souris
    int button, xold, yold;    

    struct camera *cam;
    struct light *light;
    vec3 centroid;
    
    //Plateau
    struct dtile **tiles;
    int tile_count;

    struct dpenguin **penguins;
    int nb_peng_alloc;
    int penguin_count;

    struct model *penguin_model;
    struct texture **penguin_tex;
    int penguin_tex_count;

    // Affichage Direction
    int linked;
    int **link;
    int activeLink;

    // move's list/queue
    struct record *rec;
    int autoplay;


    struct list *tex_list;
    struct list *mod_list;

    int mouseclick_mode;
};

extern struct scene scene;

#endif //DISPLAY_H
