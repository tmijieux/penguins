/**
 * @file object.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "penguins_opengl.h"

#include "utils/vec.h"
#include "utils/math.h"

#include "d3v/d3v.h"
#include "d3v/object.h"
#include "d3v/model.h"
#include "d3v/texture.h"

/**
 * Description d'un objet.
 */
struct object {
    vec3 loc;
    vec3 rot;
    vec3 scale;
    vec3 color;
    struct model *model;
    GLuint tex_id;
    int textured;
    int colored;
    int hidden;

    int shaderProgram;
    int shaderModelLocation;
    char *name;
};

/**
 * Création d'un objet.
 * @param m - Le modèle de l'objet.
 * @param t - La texture de l'objet.
 * @param loc - Position de l'objet.
 * @param rot - Orientation de l'objet.
 * @param scale - Echelle de l'objet.
 * @return struct object * - L'objet créé.
 */
object_t*
d3v_object_create(
    model_t *model,
    texture_t* tex,
    vec3 loc,
    vec3 rot,
    vec3 scale,
    int shader_program,
    const char *name)
{
    object_t *obj = calloc(1, sizeof(*obj));
    obj->loc = loc;
    obj->rot = rot;
    obj->scale = scale;
    obj->model = model;
    if (tex != NULL) {
        obj->tex_id = texture_get_id(tex);
        obj->textured = 1;
    } else {
        obj->textured = 0;
    }
    obj->colored = !obj->textured;
    obj->hidden = 0;
    obj->shaderProgram = shader_program;
    obj->shaderModelLocation = glGetUniformLocation(shader_program, "model");
    obj->name = strdup(name);

    return obj;
}

/**
 * Obtenir la position d'un objet.
 * @param obj - Un objet.
 * @param loc - Position de l'objet.
 */
void d3v_object_get_position(struct object *obj, vec3 *loc)
{
    *loc = obj->loc;
}

/**
 * Changer la position d'un objet.
 * @param obj - Un objet.
 * @param loc - Position de l'objet.
 */
void d3v_object_set_position(struct object *obj, vec3 loc)
{
    obj->loc = loc;
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

float d3v_object_get_orientationY(struct object *obj)
{
    return obj->rot.y;
}

void d3v_object_set_orientationY(struct object *obj, float y)
{
    obj->rot.y = y;
}


/**
 * Déplacer un objet.
 * @param obj - Un objet.
 * @param loc - Vecteur de déplacement.
 */
void d3v_object_translate(struct object *obj, vec3 loc)
{
    obj->loc.x += loc.x;
    obj->loc.y += loc.y;
    obj->loc.z += loc.z;
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
    if (obj == NULL || obj->hidden) {
         return;
    }

    if (obj->textured) {
        HANDLE_GL_ERROR(glBindTexture(GL_TEXTURE_2D, obj->tex_id));
    } else {
        HANDLE_GL_ERROR(glBindTexture(GL_TEXTURE_2D, 0));
    }

    mat4 t, rx,ry,rz, s;
    make_translation_matrix(obj->loc, &t);

    make_rotation_matrix((vec3){1,0,0}, degree_to_radian(obj->rot.x), &rx);
    make_rotation_matrix((vec3){0,1,0}, degree_to_radian(obj->rot.y), &ry);
    make_rotation_matrix((vec3){0,0,1}, degree_to_radian(obj->rot.z), &rz);

    make_scale_matrix(obj->scale, &s);


    mat4 model, tmp1,tmp2;

    // v = T * RX * RY * RZ * S * v
    mm_multiply(&t, &rx, &tmp1);
    mm_multiply(&tmp1, &ry, &tmp2);
    mm_multiply(&tmp2, &rz, &tmp1);
    mm_multiply(&tmp1, &s, &model);

    HANDLE_GL_ERROR(glUseProgram(obj->shaderProgram));
    HANDLE_GL_ERROR(glUniformMatrix4fv(obj->shaderModelLocation, 1, GL_TRUE, model.m));
    model_draw(obj->model);
}

/**
 * Comparer la position de l'objet avec une position.
 * @param obj - Un objet.
 * @param loc - Position à étudier.
 * @return int - 1 Si position identique.
 */
int d3v_object_pos_equal(struct object *obj, vec3 loc)
{
    return obj->loc.x == loc.x &&
    obj->loc.y == loc.y &&
    obj->loc.z == loc.z;
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
    free(obj->name);
    memset(obj, 0, sizeof*obj);
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

const char* d3v_object_get_name(object_t *obj)
{
    return obj->name;
}
