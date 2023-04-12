#ifndef D3V_SCENE_H
#define D3V_SCENE_H

#include <utils/vec.h>
#include <utils/symbol_visibility.h>

#include <d3v/d3v.h>


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

extern struct scene scene __internal;

__internal void d3v_scene_init(int obj_count_clue);
__internal void d3v_scene_exit(void);
__internal void d3v_scene_start(vec3 *first_look);

#endif // D3V_SCENE_H
