/**
 * @file object.c
 */

#include <stdio.h>
#include <stdlib.h>

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>

#include "utils/vec.h"

#include "d3v/d3v.h"
#include "d3v/object.h"
#include "d3v/model.h"
#include "d3v/texture.h"

/**
 * Description d'un objet.
 */
struct object {
    vec3 pos;
    vec3 rot;
    vec3 scale;
    vec3 color;
    struct model *m;
    GLuint tex_id;
    int textured;
    int colored;
    int hidden;
};

/**
 * Création d'un objet.
 * @param m - Le modèle de l'objet.
 * @param t - La texture de l'objet.
 * @param pos - Position de l'objet.
 * @param rot - Orientation de l'objet.
 * @param scale - Echelle de l'objet.
 * @return struct object * - L'objet créé.
 */
struct object*
d3v_object_create(struct model *m, struct texture* t,
	      vec3 pos, vec3 rot, vec3 scale)
{
    struct object *obj = malloc(sizeof(*obj));
    obj->pos = pos;
    obj->rot = rot;
    obj->scale = scale;
    obj->m = m;
    if (t != NULL) {
	obj->tex_id = texture_get_id(t);
	obj->textured = 1;
    } else {
	obj->textured = 0;
    }
    obj->colored = 0;
    obj->hidden = 0;
    return obj;
}

/**
 * Obtenir la position d'un objet.
 * @param obj - Un objet.
 * @param pos - Position de l'objet.
 */
void d3v_object_get_position(struct object *obj, vec3 *pos)
{
    *pos = obj->pos;
}

/**
 * Changer la position d'un objet.
 * @param obj - Un objet.
 * @param pos - Position de l'objet.
 */
void d3v_object_set_position(struct object *obj, vec3 pos)
{
    obj->pos = pos;
}

/**
 * Changer l'orientation d'un objet.
 * @param obj - Un objet.
 * @param rot - Orientation de l'objet.
 */
void d3v_object_set_orientation(struct object *obj, vec3 rot)
{
    obj->rot = rot;
}

/**
 * Obtenir l'orientation d'un objet.
 * @param obj - Un objet.
 * @param rot - Orientation de l'objet.
 */
void d3v_object_get_orientation(struct object *obj, vec3 *rot)
{
    *rot = obj->rot;
}

/**
 * Déplacer un objet.
 * @param obj - Un objet.
 * @param pos - Vecteur de déplacement.
 */
void d3v_object_translate(struct object *obj, vec3 pos)
{
    obj->pos.x += pos.x;
    obj->pos.y += pos.y;
    obj->pos.z += pos.z;
}

/**
 * Agrandir ou réduire la taille d'un objet.
 * @param obj - Un objet.
 * @param scale - Vecteur de transformation.
 */
void d3v_object_scale(struct object *obj, vec3 scale)
{
    obj->scale.x *= scale.x;
    obj->scale.y *= scale.y;
    obj->scale.z *= scale.z;
}

/**
 * Dessiner un objet.
 * @param obj - Un objet.
 */
void d3v_object_draw(struct object *obj)
{
    if (obj == NULL || obj->hidden)
	return;

    HANDLE_GL_ERROR(glMatrixMode(GL_MODELVIEW));
    HANDLE_GL_ERROR(glPushMatrix());
    HANDLE_GL_ERROR(glTranslated(obj->pos.x, obj->pos.y, obj->pos.z));
    HANDLE_GL_ERROR(glRotated(obj->rot.x, 1., 0., 0.));
    HANDLE_GL_ERROR(glRotated(obj->rot.y, 0., 1., 0.));
    HANDLE_GL_ERROR(glRotated(obj->rot.z, 0., 0., 1.));
    HANDLE_GL_ERROR(glScaled(obj->scale.x, obj->scale.y, obj->scale.z));

    if (obj->colored) {
	HANDLE_GL_ERROR(glColor3d(obj->color.x, obj->color.y, obj->color.z));
    }

    if (obj->textured) {
	HANDLE_GL_ERROR(glEnable(GL_TEXTURE_2D));
        HANDLE_GL_ERROR(glBindTexture(GL_TEXTURE_2D, obj->tex_id));
    } else {
	HANDLE_GL_ERROR(glBindTexture(GL_TEXTURE_2D, 0));
    }
    model_draw(obj->m);

    if (obj->textured)
	HANDLE_GL_ERROR(glDisable(GL_TEXTURE_2D));
    HANDLE_GL_ERROR(glPopMatrix());
}

/**
 * Comparer la position de l'objet avec une position.
 * @param obj - Un objet.
 * @param pos - Position à étudier.
 * @return int - 1 Si position identique.
 */
int d3v_object_pos_equal(struct object *obj, vec3 pos)
{
    return obj->pos.x == pos.x &&
	obj->pos.y == pos.y &&
	obj->pos.z == pos.z;
}

/**
 * Cacher un objet.
 * @param obj - Un objet.
 */
void d3v_object_hide(struct object *obj)
{
    obj->hidden = 1;
}

/**
 * Révéler un objet.
 * @param obj - Un objet.
 */
void d3v_object_reveal(struct object *obj)
{
    obj->hidden = 0;
}

/**
 * Libérer la mémoire occupé par un objet.
 * @param obj - Un objet.
 */
void d3v_object_free(struct object *obj)
{
    free(obj);
}

/**
 * Savoir si un objet est caché.
 * @param obj - Un objet.
 * @return int - 1 si est caché. Autre sinon.
 */
int d3v_object_is_hidden(struct object *obj)
{
    return obj->hidden;
}
