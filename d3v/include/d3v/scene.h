#ifndef D3V_SCENE_H
#define D3V_SCENE_H

#include "utils/vec.h"
#include "utils/symbol_visibility.h"
#include "d3v/user_callback.h"
#include "d3v/camera.h"
#include "d3v/object.h"
#include "d3v/light.h"

typedef struct scene {
    int key;
    // variables de la souris
    int button, xold, yold;

    camera_t *camera;
    light_t *light;

    object_t **object_buf;
    int object_count;

    vec3 first_look;

    exit_callback_t exit_callback;
    draw_callback_t draw_callback;
    mouse_callback_t mouse_callback;
    key_input_callback_t key_input_callback;
} scene_t;

__internal extern scene_t scene;

__internal void d3v_scene_module_init(int obj_count_clue);
__internal void d3v_scene_module_exit(void);

void d3v_add_object(object_t *obj);

#endif // D3V_SCENE_H
