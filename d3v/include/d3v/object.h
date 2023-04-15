#ifndef D3V_OBJECT_H
#define D3V_OBJECT_H

#include <stdint.h>

#include "utils/vec.h"
#include "d3v/texture.h"
#include "d3v/model.h"

struct object;
typedef struct object object_t;

object_t* d3v_object_create(
    model_t *m, texture_t* t,
    vec3 pos, vec3 rot, vec3 scale,
    int shader_program,
    const char *name
);
void d3v_object_free(object_t *obj);

void d3v_object_get_position(object_t *obj, vec3 *pos);
void d3v_object_set_position(object_t *obj, vec3 pos);

void d3v_object_get_orientation(object_t *obj, vec3 *rot);
void d3v_object_set_orientation(object_t *obj, vec3 rot);

float d3v_object_get_orientationY(object_t *obj);
void d3v_object_set_orientationY(object_t *obj, float y);


void d3v_object_translate(object_t *obj, vec3 pos);
void d3v_object_scale(object_t *obj, vec3 scale);

void d3v_object_draw(object_t *obj);
void d3v_object_hide(object_t *obj);
void d3v_object_reveal(object_t *obj);

const char* d3v_object_get_name(object_t *obj);

int d3v_object_pos_equal(object_t *obj, vec3 pos);
int d3v_object_is_hidden(object_t *obj);

#endif // D3V_OBJECT_H
