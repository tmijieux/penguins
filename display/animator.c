#include <stdio.h>
#include <stdlib.h>

#include "utils/vec.h"
#include "d3v/d3v.h"
#include "display/animator.h"
#include "display/dtile.h"
#include "display/dpenguin.h"

#define MAX_ANIMATION 10

/**
 * Description d'une animation.
 */
struct animation {
    void *obj; // targeted object
    int type; //0=penguin, 1=tile
    struct vec3 trans;// elementary translation vector at each frame
    struct vec3 transDest; // final position that should eventually be adopted by the object
    int doTrans; //1= animation step must translate the object
    float rot; // elementary rotation angle at each frame
    float rotDest; // final rotation angle that should eventually be adopted by the object
    int doRot; // 1= animation step must rotate the object
    int total_move; // animation duration in frame
    int current_move; // current frame of animation (between 0 and total_move)
    int hide;
    // 1 means object should be hidden when animation is executed
    // (and remain hidden afterwards)
    // 0 means object should be revealed when animation is executed
    // (and remains visible afterwards)
};

/**
 * Gestion des animations.
 */
struct animator {
    struct animation animations[MAX_ANIMATION];
    int size; // number of animation
    int capacity; // maximum number of animation
    int pos; // current animation that is executed
    int animationLocked;
    // 0=animation can run, but cannot be edited (all edition methods will fail)
    // 1=animation cannot run and is in preparation phase

    int addLocked;
    // 1=animation is currently running and cannot be locked for modification
};
static struct animator animator;

/**
 * Initialisation du module animation.
 */
void anim_init(void)
{
    animator.size = 0;
    animator.capacity = MAX_ANIMATION;
    animator.pos = 0;
    animator.animationLocked = 0;
    animator.addLocked = 0;
}

/**
 * Initialisation d'un vec3 à 0,0,0.
 * @param - Vec3 à initialiser.
 */
void anim_init_vec3(vec3 * v)
{
    *v = (vec3) {0};
}

/**
 * Active le mode préparation d'une séquence d'animations.
 * @return int - 1 Si l'activation a fonctionné.
 */
int anim_prepare(void)
{
    if (animator.addLocked || animator.size == animator.capacity) {
	return 0;
    }
    animator.animationLocked = 1;
    return 1;
}

/**
 * Active le mode lecture d'une séquence d'animations.
 */
void anim_launch(void)
{
    animator.animationLocked = 0;
    animator.addLocked = 1;
}

/**
 * Enclenche l'ajout d'un nouveau mouvement dans la séquence.
 * @param o - Élément subissant le mouvement.
 * @param type - Type de l'élément. 0=penguins, 1=tile
 * @param total - Durée total du mouvement. (frame)
 * @return int - 1 Si l'ajout a fonctionné.
 */
int anim_new_movement(void *obj, int type, int total)
{
    if (!animator.animationLocked) {
	return 0;
    }
    int s = animator.size;
    struct animation *anim = &animator.animations[s];

    anim->obj = obj;
    anim->type = type;
    anim->total_move = total;
    anim->current_move = 0;
    anim->hide = -1;

    anim_init_vec3(&anim->trans);
    anim_init_vec3(&anim->transDest);

    anim->rot = 0;
    anim->rotDest = 0;
    anim->doRot = 0;
    anim->doTrans = 0;
    return 1;
}

/**
 * Enregistrement de la translation dans le mouvement à ajouter.
 * @param dest - Destination de la translation.
 * @return int - 1 Si l'enregistrement a fonctionné.
 */
int anim_set_translation(vec3 dest)
{
    if (!animator.animationLocked) {
	return 0;
    }

    int s = animator.size;
    vec3 pen_pos;
    dpenguin_get_position(animator.animations[s].obj, &pen_pos);

    int total = animator.animations[s].total_move;
    animator.animations[s].trans.x = (dest.x - pen_pos.x) / total;
    animator.animations[s].trans.y = (dest.y - pen_pos.y) / total;
    animator.animations[s].trans.z = (dest.z - pen_pos.z) / total;

    animator.animations[s].transDest = dest;
    animator.animations[s].doTrans = 1;

    return 1;
}

/**
 * Enregistrement de la rotation dans le mouvement à ajouter.
 * @param dest - Angle de destination (En degré).
 * @return int - 1 Si l'enregistrement a fonctionné.
 */
int anim_set_rotation(float dest)
{
    if (!animator.animationLocked) {
	return 0;
    }
    int s = animator.size;
    struct animation *animation = &(animator.animations[s]);
    if (dest > 360) {
	vec3 peng_pos;
	dpenguin_get_position(animation->obj, &peng_pos);
	animation->rot = (360.0 * (animation->transDest.y - peng_pos.y) / animation->total_move);
	dest = dpenguin_get_orientation(animation->obj);
    } else {
	animation->rot = (dest - dpenguin_get_orientation(animation->obj))  / animation->total_move;
    }

    animation->rotDest = dest;
    animation->doRot = 1;
    return 1;
}

/**
 * Enregistrement de l'état dans le mouvement à ajouter.
 * @param hide - 1 Pour cacher l'element, 0 pour le dévoiler.
 * @return int - 1 Si l'enregistrement a fonctionné.
 */
int anim_set_hide(int hide)
{
    if (!animator.animationLocked) {
	return 0;
    }

    animator.animations[animator.size].hide = hide;
    return 1;
}

/**
 * Ajouter l'animation dans la séquence.
 * @return int - 1 Si l'ajout a fonctionné.
 */
int anim_push_movement(void)
{
    if (!animator.animationLocked) {
	return 0;
    }

    animator.size++;
    return 1;
}

/**
 * Exécuter l'animation courrante.
 * @return int - 1 S'il reste des animations à exécuter.
 */
int anim_run(void)
{
    if (animator.animationLocked || animator.pos == animator.size) {
	return 0;
    }

    animator.addLocked = 1;
    struct animation *anim = &animator.animations[animator.pos];
    if (anim->type == 0)
    {
	if (anim->hide == 1) {
	    dpenguin_hide(anim->obj);
        } else if (anim->hide == 0) {
	    dpenguin_reveal(anim->obj);
        }

	if (anim->doTrans) {
	    vec3 pen_pos;
	    dpenguin_get_position(anim->obj, &pen_pos);
	    vec3 dest = {
		pen_pos.x + anim->trans.x,
		pen_pos.y + anim->trans.y,
		pen_pos.z + anim->trans.z
	    };

	    dpenguin_set_position(anim->obj, dest);
	}

	if (anim->doRot) {
	    float rot = dpenguin_get_orientation(anim->obj) + anim->rot;
	    dpenguin_rotate(anim->obj, rot);
	}

	++anim->current_move;
	if (anim->current_move >= anim->total_move)
        {
	    if (anim->doTrans) {
		dpenguin_set_position(anim->obj, anim->transDest);
            }
	    if (anim->doRot) {
		dpenguin_rotate(anim->obj, anim->rotDest);
            }
	}

    } else if (anim->type == 1) {

        if (anim->hide == 1) {
	    dtile_hide(anim->obj);
        }
	else if (anim->hide == 0) {
	    dtile_reveal(anim->obj);
        }

	++anim->current_move;
    }

    if (anim->current_move >= anim->total_move) {
	++animator.pos;
	if (animator.pos == animator.size) {
	    animator.pos = 0;
	    animator.size = 0;
	    animator.addLocked = 0;
	    return 0;
	}
    }
    d3v_post_redisplay();
    return 1;
}
