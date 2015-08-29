#ifndef SCENE_H
#define SCENE_H

#include <d3v.h>
#include <utils/vec.h>


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


#endif //SCENE_H
