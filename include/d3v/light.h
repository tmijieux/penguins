#ifndef LIGHT_H
#define LIGHT_H

#include <utils/symbol_visibility.h>
struct light;

__internal struct light* d3v_light_create(void);
__internal void d3v_light_update(struct light *l);
__internal void d3v_light_free(struct light *l);

#endif //LIGHT_H
