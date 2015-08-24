#ifndef MODEL_H
#define MODEL_H

struct model;
struct model *model_load_wavefront(const char *path);

void model_draw(struct model*); //must not be visible in user header !!
void model_dump(struct model*);
void model_free(struct model*);

#endif //MODEL_H
