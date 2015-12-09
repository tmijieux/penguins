#ifndef DPENGUIN_H
#define DPENGUIN_H

#include <utils/vec.h>

#include <display/dtile.h>

#include <d3v/object.h>
#include <d3v/texture.h>
#include <d3v/model.h>

struct dpenguin;

struct dpenguin*
dpenguin_create(struct model *m, struct texture *t,
		vec3 pos, double rot, double scale);

void dpenguin_draw(struct dpenguin *pen);

int dpenguin_is_on_tile(struct dpenguin *pen, struct dtile* ti);

void dpenguin_hide(struct dpenguin*);
void dpenguin_reveal(struct dpenguin*);

void dpenguin_get_position(struct dpenguin *pen, vec3 *pos);
void dpenguin_set_position(struct dpenguin *pen, vec3 pos);

double dpenguin_get_orientation(struct dpenguin *pen);
void dpenguin_rotate(struct dpenguin *pen, double a);

void dpenguin_free(struct dpenguin *pen);

#endif //DPENGUIN_H
