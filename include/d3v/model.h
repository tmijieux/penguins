#ifndef MODEL_H
#define MODEL_H

#include <utils/symbol_visibility.h>

struct model;

struct model *model_load_wavefront(const char *path);
void model_dump(struct model*);
void model_free(struct model*);

// the following is NOT part of the interface
__internal void model_draw(struct model*);

#endif //MODEL_H
