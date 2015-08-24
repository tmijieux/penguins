#ifndef DISPLAY_H
#define DISPLAY_H

#include <d3v.h>
#include <display.h>
#include <utils/vec.h>

#define WINDOW_TITLE "Penguin"
#define WINDOW_POSITION_X 200
#define WINDOW_POSITION_Y 200

#define HEIGHT  700
#define WIDTH   700

void scene_mouseclick_mode(int v);

struct scene {
    int key;
    // variables de la souris
    int button, xold, yold;    

    struct camera *cam;
    struct light *light;

    struct object **object_buf;
    int object_count;

    vec3 first_look;

    void (*exit_callback)(void);
    void (*draw_callback)(void);
    void (*mouse_callback)(int,int,int,int);
    void (*spe_input_callback)(int,int,int);
    void (*key_input_callback)(int,int,int);
    
};

extern struct scene scene;

void d3v_scene_init(int obj_count_clue);
void d3v_scene_exit(void);
void d3v_scene_start(vec3 *first_look);
void d3v_init_glut_callback(void);


#endif //DISPLAY_H
