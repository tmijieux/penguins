#ifndef ANIMATOR_H
#define ANIMATOR_H

#include <utils/vec.h>

#define NB_MOVE 40

void anim_init(void);
int anim_new_movement(void *o, int type, int total);
int anim_set_translation(vec3 dest);
int anim_set_rotation(float dest);
int anim_set_hide(int hide);
int anim_push_movement(void);
int anim_prepare(void);
void anim_launch(void);
int anim_run(void);

#endif //ANIMATOR_H
