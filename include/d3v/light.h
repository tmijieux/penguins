#ifndef LIGHT_H
#define LIGHT_H

struct light;

struct light *d3v_light_create(void);
void d3v_light_update(struct light *l);
void d3v_light_free(struct light *l);

#endif //LIGHT_H
