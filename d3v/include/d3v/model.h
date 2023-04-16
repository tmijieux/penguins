#ifndef D3V_MODEL_H
#define D3V_MODEL_H

#include <stdlib.h> // for size_t
#include "utils/symbol_visibility.h"

struct model;
typedef struct model model_t;

model_t *model_load_wavefront(const char *path);

model_t *create_model_from_data(
    void *data, uint32_t nb_vertices, size_t buffer_size, int gl_primitive,
    int have_uv, int have_color, int have_normals,
    size_t uv_offset, size_t color_offset, size_t normals_offset, int stride);


void model_dump(struct model*);
void model_free(struct model*);

// the following is NOT part of the interface
__internal void model_draw(model_t*);

#endif // D3V_MODEL_H
