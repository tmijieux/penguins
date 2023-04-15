#ifndef D3V_SCENE_H
#define D3V_SCENE_H

#include "utils/vec.h"
#include "utils/symbol_visibility.h"
#include "d3v/d3v.h"
#include "d3v/camera.h"
#include "d3v/object.h"


struct scene {
    int key;
    // variables de la souris
    int button, xold, yold;

    camera_t *camera;
    struct light *light;

    struct object **object_buf;
    int object_count;

    vec3 first_look;

    void (*exit_callback)(void);
    void (*draw_callback)(void);
    void (*mouse_callback)(int,int,int,int);
    void (*key_input_callback)(int,int,int);
};
typedef struct scene scene_t;

__internal extern scene_t scene;

__internal void d3v_scene_module_init(int obj_count_clue);
__internal void d3v_scene_module_exit(void);

void d3v_add_object(object_t *obj);

#endif // D3V_SCENE_H
