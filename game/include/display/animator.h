#ifndef ANIMATOR_H
#define ANIMATOR_H

#include <utils/vec.h>

#define NB_MOVE 40 // at 60fps is basically 666ms

typedef enum anim_object_type
{
    ANIM_O_PENGUIN,
    ANIM_O_TILE,
} anim_object_type_t;



void init_anim_module(void);

int anim_new_movement(void *o, anim_object_type_t type, int total);
int anim_set_translation(vec3 dest);
int anim_set_rotation(float dest);
int anim_set_flip(void);
int anim_set_hide(int hide);
int anim_push_movement(void);
int anim_prepare(void);
void anim_launch(void);
int anim_run(void);

#endif //ANIMATOR_H
