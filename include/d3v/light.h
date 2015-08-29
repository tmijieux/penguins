#ifndef LIGHT_H
#define LIGHT_H

#include <utils/so.h>
struct light;

__so_local struct light *d3v_light_create(void);
__so_local void d3v_light_update(struct light *l);
__so_local void d3v_light_free(struct light *l);

#endif //LIGHT_H
