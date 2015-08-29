#ifndef SCENE_H
#define SCENE_H

#include <d3v.h>
#include <utils/vec.h>
#include <utils/so.h>

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

extern struct scene scene __so_local;

void d3v_scene_init(int obj_count_clue) __so_local;
void d3v_scene_exit(void) __so_local;
void d3v_scene_start(vec3 *first_look) __so_local;

#endif //SCENE_H
