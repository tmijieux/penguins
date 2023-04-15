#include <stdio.h>
#include <stdlib.h>

#include <GL/glu.h>

#include "d3v/light.h"
#include "d3v/d3v.h"

/**
 * @file light.c
 */

/**
 * Description de la lumière.
 */
struct light {
/** Paramètres de la lumière **/
    GLfloat L0pos[4];
    GLfloat L0dif[4];
    GLfloat L0dir[4];
    GLfloat Mspec[4];
    GLfloat Mshiny;
};

/**
 * Création de la lumière.
 * @return struct light * - La lumière.
 */
  __internal
struct light *d3v_light_create(void)
{
    struct light *l = malloc(sizeof(*l));

    // position
    l->L0pos[0] = 0.0;
    l->L0pos[1] = 1.0;
    l->L0pos[2] = -0.5;
    l->L0pos[3] = 0.0;

    // dif
    l->L0dif[0] = 1.0;
    l->L0dif[1] = 1.0;
    l->L0dif[2] = 1.0;

    // dir
    l->L0dir[0] = 0.0;
    l->L0dir[1] = -1.5;
    l->L0dir[2] = -1.0;

    // spec
    l->Mspec[0] = 1.0;
    l->Mspec[1] = 1.0;
    l->Mspec[2] = 1.0;

    // shiny
    l->Mshiny = 80;


    /* GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 }; */
    /* GLfloat mat_shininess[] = { 100.0 }; */


    /* glShadeModel(GL_SMOOTH); */

    /* glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular); */
    /* glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess); */
    /* //glLightfv(GL_LIGHT0, GL_POSITION, l->L0pos); */
    /* //glLightfv(GL_LIGHT1, GL_POSITION, l->L0pos); */

    /* glEnable(GL_LIGHT0); */
    /* //glEnable(GL_LIGHT1); */

    return l;
}

/**
 * Mise à jour de la lumière.
 * @param l - La lumière.
 */
__internal
void d3v_light_update(struct light *l)
{
    //GLfloat mat[] = { 0.0, -1.0, 0.5, 0.0 };
    /* HANDLE_GL_ERROR(glLightfv(GL_LIGHT0, GL_POSITION, l->L0pos)); */
    // HANDLE_GL_ERROR(glLightfv(GL_LIGHT1, GL_POSITION, mat));
}

/**
 * Libérer la mémoire utilisé par la lumière.
 * x@param l - La lumière.
 */
 __internal
void d3v_light_free(struct light *l)
{
    free(l);
}
