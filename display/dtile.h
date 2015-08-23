#ifndef DTILE_H
#define DTILE_H

#include <utils/vec.h>

#include "texture.h"
#include "model.h"

struct dtile;

struct dtile* dtile_create(struct model *m, struct texture *t,
			   vec3 pos, double rot,
			   double scale, int fish_count);
			   
void dtile_draw(struct dtile* ti);
void dtile_get_position(struct dtile *ti, vec3 *pos);

void dtile_hide(struct dtile *ti);
void dtile_reveal(struct dtile *ti);

void dtile_free(struct dtile *ti);

void dtile_set_penguin(struct dtile *, int penguin_id);
int dtile_get_penguin(const struct dtile *ti);
    
#endif //DTILE_H
