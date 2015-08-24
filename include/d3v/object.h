#ifndef OBJECT_H
#define OBJECT_H

#include <utils/vec.h>

#include "texture.h"
#include "model.h"

struct object;

struct object* d3v_object_create(struct model *m, struct texture* t,
			     vec3 pos, vec3 rot, vec3 scale);
void d3v_object_free(struct object *obj);

void d3v_object_get_position(struct object *obj, vec3 *pos);
void d3v_object_set_position(struct object *obj, vec3 pos);

void d3v_object_get_orientation(struct object *obj, vec3 *rot);
void d3v_object_set_orientation(struct object *obj, vec3 rot);
    
void d3v_object_translate(struct object *obj, vec3 pos);
void d3v_object_scale(struct object *obj, vec3 scale);
		  
void d3v_object_draw(struct object *obj);
void d3v_object_hide(struct object *obj);
void d3v_object_reveal(struct object *obj);

int d3v_object_pos_equal(struct object *obj, vec3 pos);
int d3v_object_is_hidden(struct object *obj);

#endif //OBJECT_H
