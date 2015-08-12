#ifndef OBJECT_H
#define OBJECT_H

#include <utils/vec.h>

#include "texture.h"
#include "model.h"

struct object;

struct object* object_create(struct model *m, struct texture* t,
			     vec3 pos, vec3 rot, vec3 scale);
void object_free(struct object *obj);

void object_get_position(struct object *obj, vec3 *pos);
void object_set_position(struct object *obj, vec3 pos);

void object_get_orientation(struct object *obj, vec3 *rot);
void object_set_orientation(struct object *obj, vec3 rot);
    
void object_translate(struct object *obj, vec3 pos);
void object_scale(struct object *obj, vec3 scale);
		  
void object_draw(struct object *obj);
void object_hide(struct object *obj);
void object_reveal(struct object *obj);

int object_pos_equal(struct object *obj, vec3 pos);
int object_is_hidden(struct object *obj);

#endif //OBJECT_H
