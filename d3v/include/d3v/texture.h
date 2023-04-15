#ifndef D3V_TEXTURE_H
#define D3V_TEXTURE_H

struct texture;
typedef struct texture texture_t;

texture_t *texture_load(const char *path);
void texture_free(texture_t *t);
unsigned int texture_get_id(texture_t *t);

#endif // D3V_TEXTURE_H
