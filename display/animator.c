/**
 * @file: animator.c 
 */

#include <stdio.h>
#include <stdlib.h>

#include <utils/vec.h>

#include <display/animator.h>
#include <display/dtile.h>
#include <display/dpenguin.h>

#define MAX_ANIMATION 10

/**
 * Description d'une animation.
 */
struct animation {
    void *o;
    int type;
    struct vec3 trans;
    struct vec3 transDest;
    int doTrans;
    float rot;
    float rotDest;
    int doRot;
    int total_move;
    int current_move;
    int hide;
};

/**
 * Gestion des animations.
 */
static struct {
    struct animation animations[MAX_ANIMATION];
    int size;
    int capacity;
    int pos;
    int animationLocked;
    int addLocked;
} animator;

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
    *v = (vec3) {
	0};
}

/**
 * Active le mode préparation d'une séquence d'animations.
 * @return int - 1 Si l'activation a fonctionné.
 */
int anim_prepare(void)
{
    if (animator.addLocked || animator.size == animator.capacity)
	return 0;
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
 * @param o - Element subissant le mouvement.
 * @param type - Type de l'élément.
 * @param total - Durée total du mouvement.
 * @return int - 1 Si l'ajout a fonctionné.
 */
int anim_new_movement(void *o, int type, int total)
{
    if (!animator.animationLocked)
	return 0;
    int s = animator.size;

    animator.animations[s].o = o;
    animator.animations[s].type = type;
    animator.animations[s].total_move = total;
    animator.animations[s].current_move = 0;
    animator.animations[s].hide = -1;
    anim_init_vec3(&animator.animations[s].trans);
    anim_init_vec3(&animator.animations[s].transDest);
    animator.animations[s].rot = 0;
    animator.animations[s].rotDest = 0;
    animator.animations[s].doRot = 0;
    animator.animations[s].doTrans = 0;
    return 1;
}

/**
 * Enregistrement de la translation dans le mouvement à ajouter. 
 * @param dest - Destination de la translation.
 * @return int - 1 Si l'enregistrement a fonctionné.
 */
int anim_set_translation(vec3 dest)
{
    if (!animator.animationLocked)
	return 0;

    int s = animator.size;
    vec3 pen_pos;
    dpenguin_get_position(animator.animations[s].o, &pen_pos);

    animator.animations[s].trans.x =
	(dest.x - pen_pos.x) / animator.animations[s].total_move;
    animator.animations[s].trans.y =
	(dest.y - pen_pos.y) / animator.animations[s].total_move;
    animator.animations[s].trans.z =
	(dest.z - pen_pos.z) / animator.animations[s].total_move;

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
    if (!animator.animationLocked)
	return 0;
    int s = animator.size;
    if (dest > 360) {

	vec3 peng_pos;
	dpenguin_get_position(animator.animations[s].o, &peng_pos);

	animator.animations[s].rot =
	    (360.0 *   (animator.animations[s].transDest.y - peng_pos.y) / animator.animations[s].total_move);
	dest = dpenguin_get_orientation(animator.animations[s].o);
    } else
	animator.animations[s].rot =
	    (dest - dpenguin_get_orientation(animator.animations[s].o))
	    / animator.animations[s].total_move;

    animator.animations[s].rotDest = dest;
    animator.animations[s].doRot = 1;
    return 1;
}

/**
 * Enregistrement de l'état dans le mouvement à ajouter. 
 * @param hide - 1 Pour cacher l'element, 0 pour le dévoiler.
 * @return int - 1 Si l'enregistrement a fonctionné.
 */
int anim_set_hide(int hide)
{
    if (!animator.animationLocked)
	return 0;

    animator.animations[animator.size].hide = hide;
    return 1;
}

/**
 * Ajouter l'animation dans la séquence. 
 * @return int - 1 Si l'ajout a fonctionné.
 */
int anim_push_movement(void)
{
    if (!animator.animationLocked)
	return 0;

    animator.size++;
    return 1;
}

/**
 * Exécuter l'animation courrante. 
 * @return int - 1 S'il reste des animations à exécuter.
 */
int anim_run(void)
{
    if (animator.animationLocked || animator.pos == animator.size)
	return 0;

    animator.addLocked = 1;
    if (animator.animations[animator.pos].type == 0) {
	if (animator.animations[animator.pos].hide == 1)
	    dpenguin_hide(animator.animations[animator.pos].o);
	else if (animator.animations[animator.pos].hide == 0)
	    dpenguin_reveal(animator.animations[animator.pos].o);

	if (animator.animations[animator.pos].doTrans) {
	    vec3 pen_pos;
	    dpenguin_get_position(animator.animations[animator.pos].o,
				  &pen_pos);
	    vec3 dest = {
		pen_pos.x + animator.animations[animator.pos].trans.x,
		pen_pos.y + animator.animations[animator.pos].trans.y,
		pen_pos.z + animator.animations[animator.pos].trans.z
	    };

	    dpenguin_set_position(animator.animations[animator.pos].o,
				  dest);
	}

	if (animator.animations[animator.pos].doRot) {
	    float rot =
		dpenguin_get_orientation(animator.animations[animator.pos].
					 o)
		+ animator.animations[animator.pos].rot;
	    dpenguin_rotate(animator.animations[animator.pos].o, rot);
	}

	++animator.animations[animator.pos].current_move;
	if (animator.animations[animator.pos].current_move >=
	    animator.animations[animator.pos].total_move) {
	    if (animator.animations[animator.pos].doTrans)
		dpenguin_set_position(animator.animations[animator.pos].o,
				      animator.animations[animator.pos].
				      transDest);
	    if (animator.animations[animator.pos].doRot)
		dpenguin_rotate(animator.animations[animator.pos].o,
				animator.animations[animator.pos].rotDest);
	}

    } else if (animator.animations[animator.pos].type == 1) {
	if (animator.animations[animator.pos].hide == 1)
	    dtile_hide(animator.animations[animator.pos].o);
	else if (animator.animations[animator.pos].hide == 0)
	    dtile_reveal(animator.animations[animator.pos].o);

	++animator.animations[animator.pos].current_move;
    }

    if (animator.animations[animator.pos].current_move >=
	animator.animations[animator.pos].total_move) {
	++animator.pos;
	if (animator.pos == animator.size) {
	    animator.pos = 0;
	    animator.size = 0;
	    animator.addLocked = 0;
	    return 0;
	}
    }
    return 1;
}
