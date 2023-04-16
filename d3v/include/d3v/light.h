#ifndef D3V_LIGHT_H
#define D3V_LIGHT_H

#include <utils/symbol_visibility.h>

typedef struct light light_t;

__internal light_t* d3v_light_create(void);
__internal void d3v_light_update(light_t *l);
__internal void d3v_light_free(light_t *l);

#endif // D3V_LIGHT_H
