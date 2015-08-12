#ifndef LIGHT_H
#define LIGHT_H

struct light;

struct light *light_create(void);
void light_update(struct light *l);
void light_free(struct light *l);

#endif //LIGHT_H
