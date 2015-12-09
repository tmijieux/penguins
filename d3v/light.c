#include <stdio.h>
#include <stdlib.h>
#include <GL/glu.h>
#include <d3v/light.h>
#include <utils/vec.h>

/**
 * @file light.c
 */

/**
 * Description de la lumière.
 */

struct light {
    union {
	struct {
	    vec4f pos;
	    vec4f dif;
	    vec4f dir;
	    vec4f Mspec;
	    GLfloat Mshiny;
	};
	struct {
	    GLfloat Lpos[4];
	    GLfloat Ldif[4];
	    GLfloat Ldir[4];
	    GLfloat LMspec[4];
	};
    };
};

/**
 * Création de la lumière.
 * @return struct light * - La lumière.
 */
__internal
struct light *d3v_light_create(void)
{
    struct light *l = malloc(sizeof(*l));
    l->pos =   (vec4f) { 2.0,  2.0,  2.0, 0. };
    l->dif =   (vec4f) { 1.0,  1.0,  1.0, 0.0 };
    l->dir =   (vec4f) { -1.0, -1.0, -1.0, 0.0 };
    l->Mspec = (vec4f) { 1.0,  1.0,  1.0, 0.0 };
    l->Mshiny = 80.;


    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_shininess[] = { 100.0 };
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    //glLightfv(GL_LIGHT0, GL_POSITION, l->L0pos);
    //glLightfv(GL_LIGHT1, GL_POSITION, l->L0pos);

    
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    
    glShadeModel(GL_SMOOTH);
	
    //glEnable(GL_LIGHT1);
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
     glLightfv(GL_LIGHT0, GL_POSITION, l->Lpos);
     //glLightfv(GL_LIGHT1, GL_POSITION, mat);
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
