#ifndef MODEL_H
#define MODEL_H

#include <utils/so.h>

struct model;

struct model *model_load_wavefront(const char *path);
void model_dump(struct model*);
void model_free(struct model*);



// the following is NOT part of the interface

__so_local void model_draw(struct model*);

#endif //MODEL_H
