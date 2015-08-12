#ifndef MODELS_H
#define MODELS_H

struct model;
struct model *model_load_wavefront(const char *path);

void model_draw(struct model*);
void model_dump(struct model*);
void model_free(struct model*);

#endif //MODELS_H
