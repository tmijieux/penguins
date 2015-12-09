#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/gl.h>

#include <display/texture.h>

void texture_free(struct texture *t);
GLuint texture_get_id(struct texture *t);

#endif //TEXTURE_H
