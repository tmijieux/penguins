#ifndef D3V_LIGHT_H
#define D3V_LIGHT_H

#include <utils/symbol_visibility.h>

struct light;

__internal struct light* d3v_light_create(void);
__internal void d3v_light_update(struct light *l);
__internal void d3v_light_free(struct light *l);

#endif // D3V_LIGHT_H
