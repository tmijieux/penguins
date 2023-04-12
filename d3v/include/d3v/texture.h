#ifndef D3V_TEXTURE_H
#define D3V_TEXTURE_H

struct texture;

struct texture *texture_load(const char *path);
void texture_free(struct texture *t);
unsigned int texture_get_id(struct texture *t);

#endif // D3V_TEXTURE_H
