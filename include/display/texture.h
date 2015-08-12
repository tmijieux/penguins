#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/gl.h>

struct texture;

struct texture *texture_load(const char *path);
void texture_free(struct texture *t);

GLuint texture_get_id(struct texture *t);

#endif //TEXTURE_H
