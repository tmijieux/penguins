#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "utils/vec.h"
#include "utils/math.h"
#include "d3v/d3v.h"
#include "d3v/object.h"
#include "display/animator.h"

#define MAX_ANIMATION_STEP 20
#ifndef _WIN32
#define max(x,y) ({                              \
        __typeof__((x)) _x = (x);                 \
        __typeof__((y)) _y = (y);                 \
        _x > _y ? _x : _y; })
#endif

/**
 * Description d'une animation.
 */
typedef struct animation_step {
    struct object *obj; // targeted object
    anim_object_type_t type; //0=penguin, 1=tile

    int shouldTranslate; //1= animation step must translate the object
    int shouldRotate; // 1= animation step must rotate the object
    int shouldFlip; // wwhoooo 360 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    int shouldHide;
    // 1 means object should be hidden when animation is executed
    // (and remain hidden afterwards)
    // 0 means object should be revealed when animation is executed
    // (and remains visible afterwards)

    float finalAngle; // final rotation angle that should eventually be adopted by the object
    vec3 finalPosition; // final position that should eventually be adopted by the object

    int frame_length; // animation duration in frame
    int current_frame; // current frame of animation (between 0 and frame_length)
} animation_step_t;

/**
 * Gestion des animations.
 */
typedef struct animation {
    animation_step_t steps[MAX_ANIMATION_STEP];
    int nb_step; // number of animation
    int cur_step; // current animation that is executed
    int can_edit_animation;
    // 0=animation can run, but cannot be edited (all edition methods will fail)
    // 1=animation cannot run and is in preparation phase

    int animation_is_running;
    // 1=animation is currently running and cannot be locked for modification
} animation_t;
static animation_t animation;

/**
 * Initialisation du module animation.
 */
void init_anim_module(void)
{
    animation.nb_step = 0;
    animation.cur_step = 0;
    animation.can_edit_animation = 0;
    animation.animation_is_running = 0;
}

/**
 * Active le mode préparation d'une séquence d'animations.
 * @return int - 1 Si l'activation a fonctionné.
 */
int anim_prepare(void)
{
    if (animation.animation_is_running || animation.nb_step == MAX_ANIMATION_STEP) {
        return 0;
    }
    animation.can_edit_animation = 1;
    return 1;
}

/**
 * Active le mode lecture d'une séquence d'animations.
 */
void anim_launch(void)
{
    animation.can_edit_animation = 0;
    animation.animation_is_running = 1;
}

/**
 * Enclenche l'ajout d'un nouveau mouvement dans la séquence.
 * @param o - Élément subissant le mouvement.
 * @param type - Type de l'élément. 0=penguins, 1=tile
 * @param total - Durée total du mouvement. (frame)
 * @return int - 1 Si l'ajout a fonctionné.
 */
int anim_new_movement(void *obj, anim_object_type_t type, int length)
{
    if (!animation.can_edit_animation) {
        return 0;
    }
    int s = animation.nb_step;
    animation_step_t *step = &animation.steps[s];
    memset(step, 0, sizeof *step);

    step->obj = obj;
    step->type = type;
    step->frame_length = length;
    step->current_frame = 0;
    step->shouldRotate = 0;
    step->shouldTranslate = 0;
    step->shouldFlip = 0;
    step->shouldHide = 0;
    step->finalPosition = (vec3){0,0,0};
    step->finalAngle = 0;
    return 1;
}

/**
 * Enregistrement de la translation dans le mouvement à ajouter.
 * @param dest - Destination de la translation.
 * @return int - 1 Si l'enregistrement a fonctionné.
 */
int anim_set_translation(vec3 dest)
{
    if (!animation.can_edit_animation) {
        return 0;
    }

    int s = animation.nb_step;

    animation_step_t *step = &animation.steps[s];
    step->finalPosition = dest;
    step->shouldTranslate = 1;

    return 1;
}


int anim_set_flip(void)
{
    // special case where penguin is going up or down
    // make it do a 360 roll on itself !!!! because why not

   if (!animation.can_edit_animation) {
        return 0;
    }
    int s = animation.nb_step;
    animation_step_t *step = &(animation.steps[s]);

    vec3 src;
    d3v_object_get_position(step->obj, &src);
    double height = step->finalPosition.y - src.y;

    step->finalAngle = d3v_object_get_orientationY(step->obj);
    step->shouldFlip = 1;
    step->shouldRotate = 0;

    return 1;
}

/**
 * Enregistrement de la rotation dans le mouvement à ajouter.
 * @param dest - Angle de destination (En degré).
 * @return int - 1 Si l'enregistrement a fonctionné.
 */
int anim_set_rotation(float dest)
{
   if (!animation.can_edit_animation) {
        return 0;
    }
    int s = animation.nb_step;
    animation_step_t *step = &(animation.steps[s]);

    step->finalAngle = angle_normalize(dest);
    step->shouldRotate = 1;
    step->shouldFlip = 0;
    float cur = d3v_object_get_orientationY(step->obj);
    return 1;
}

/**
 * Enregistrement de l'état dans le mouvement à ajouter.
 * @param hide - 1 Pour cacher l'element, 0 pour le dévoiler.
 * @return int - 1 Si l'enregistrement a fonctionné.
 */
int anim_set_hide(int shouldHide)
{
    if (!animation.can_edit_animation) {
        return 0;
    }

    animation.steps[animation.nb_step].shouldHide = shouldHide;
    return 1;
}

/**
 * Ajouter l'animation dans la séquence.
 * @return int - 1 Si l'ajout a fonctionné.
 */
int anim_push_movement(void)
{
    if (!animation.can_edit_animation) {
        return 0;
    }
    animation.nb_step++;
    return 1;
}

/**
 * Exécuter l'animation courrante.
 * @return int - 1 S'il reste des animations à exécuter. 0 si l'animation est terminé.
 */
int anim_run(void)
{
    if (animation.can_edit_animation || animation.nb_step == 0) {
        return 0;
    }

    animation.animation_is_running = 1;
    animation_step_t *step = &animation.steps[animation.cur_step];
    if (step->type == ANIM_O_PENGUIN)
    {
        if (step->shouldHide) {
            d3v_object_hide(step->obj);
        } else {
            d3v_object_reveal(step->obj);
        }

        if (step->shouldTranslate) {
            vec3 src;
            vec3 dst = step->finalPosition;
            d3v_object_get_position(step->obj, &src);

            float nb_frame = max(step->frame_length - step->current_frame, 1);
            vec3 d = {dst.x - src.x, dst.y - src.y, dst.z - src.z};
            d.x /= nb_frame;
            d.y /= nb_frame;
            d.z /= nb_frame;

            vec3 dest = {
                src.x + d.x,
                src.y + d.y,
                src.z + d.z
            };
            d3v_object_set_position(step->obj, dest);
        }

        if (step->shouldRotate) {
            float cur = angle_normalize(d3v_object_get_orientationY(step->obj));
            float dest = angle_normalize(step->finalAngle);
            float da = dest - cur;
            int nb_step = max(step->frame_length - step->current_frame, 1);
            if (da > 180) {
                da = da - 360; // shortest path: if more than 180 go the other way
            }
            else if (da < -180) {
                da = 360 + da; // shortest path: if more than 180 go the other way
            }
            da /= nb_step;

            float rot = angle_normalize(cur + da);
            d3v_object_set_orientationY(step->obj, rot);
        }
        if (step->shouldFlip) {
            float ratio = (float)step->current_frame / (float)step->frame_length;
            float wantedAngle = step->finalAngle + ratio * 1.5 * 360;
            float rot = angle_normalize(wantedAngle);
            d3v_object_set_orientationY(step->obj, rot);
        }

        ++step->current_frame;
        if (step->current_frame >= step->frame_length)
        {
            if (step->shouldTranslate) {
                d3v_object_set_position(step->obj, step->finalPosition);
            }
            if (step->shouldRotate || step->shouldFlip) {
                d3v_object_set_orientationY(step->obj, step->finalAngle);
            }
        }
    }
    else if (step->type == ANIM_O_TILE) {
        if (step->shouldHide == 1) {
            d3v_object_hide(step->obj);
        }
        else if (step->shouldHide == 0) {
            d3v_object_reveal(step->obj);
        }
        ++step->current_frame;
    }


    d3v_request_animation_frame();

    // transition to next step
    if (step->current_frame >= step->frame_length) {
        ++animation.cur_step;
        if (animation.cur_step == animation.nb_step) {
            // if all steps are finished, reset the animation
            animation.cur_step = 0;
            animation.nb_step = 0;
            animation.animation_is_running = 0;
            return 0;
        }
    }

    return 1;
}
