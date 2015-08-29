/**
 * @file camera.c
 */

#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <GL/glu.h>
#include <utils/math.h>
#include <d3v/camera.h>

struct camera {
    // Point observé
    vec3 look;
    // Position dans la base (look, x, y ,z)
    vec3 rpos;
    vec3 up, right;

    double dis;
    // Distance par rapport au point observé

    // ay: angle autour de y (par rapport a z)
    // ar: angle autour de right (par rapport au plan xz)
    double ar, ay;
    int ortho;
    double orthosize;
};

/**
 * Rotation de la camera.
 * @param c - La camera.
 * @param ar - angle par rapport au plan xz.
 * @param ay - angle par rapport à z.
 */
static void camera_do_rotate(struct camera *c, double ar, double ay)
{
    c->rpos = (vec3) {0., 0., 1.};
    c->up = (vec3) {0., 1., 0.};
    c->right = (vec3) {1., 0., 0.};
    double *rot = rotation_matrix(c->right, ar);
    matrix_multiply(&c->up, rot);
    matrix_multiply(&c->rpos, rot);
    free(rot);
    rot = rotation_matrix((vec3) {0., 1., 0.}, ay);
    matrix_multiply(&c->up, rot);
    matrix_multiply(&c->rpos, rot);
    matrix_multiply(&c->right, rot);
    free(rot);
}

/**
 * Création de la camera.
 * @return struct camera * - La camera.
 * @param look - Position où regarde la camera.
 * @param dis - Distance de la camera.
 * @param ar - angle par rapport au plan xz.
 * @param ay - angle par rapport à z.
 * @param ortho - 1 si le mode orthogonal est activé.
 */
struct camera *d3v_camera_create(vec3 look, double dis, int ar, int ay,
			     int ortho)
{
    struct camera *c = malloc(sizeof(*c));
    c->ortho = ortho;
    c->orthosize = 1.;
    c->ar = degree_to_radian(ar);
    c->ay = degree_to_radian(ay);
    c->look = look;
    c->dis = dis;
    camera_do_rotate(c, c->ar, c->ay);
    return c;
}

/**
 * Change le mode de projection de la camera.
 * @param c - La camera.
 */
void d3v_camera_switch_ortho(struct camera *c)
{
    c->ortho = !c->ortho;
    if (!c->ortho) {
	double d = 1. / c->orthosize;
	glMatrixMode(GL_MODELVIEW);
	glScaled(d, d, d);
	c->orthosize = 1.;
    }
}

/**
 * Libère la mémoire occupé par la camera.
 * @param c - La camera.
 */ 
void d3v_camera_free(struct camera *c)
{
    free(c);
}

/**
 * Change la position où regarde la camera.
 * @param c - La camera.
 * @param look - Nouvelle position.
 */
void d3v_camera_set_look(struct camera *c, vec3 *look)
{
    c->look = *look;
}

/**
 * Change la distance de la camera.
 * @param c - La camera.
 * @param d - Distance de la camera.
 */
void d3v_camera_set_distance(struct camera *c, double d)
{
    c->dis = d;
}

/**
 * Déplacement de la camera
 * @param c - La camera.
 * @param dw - Déplacement sur la largeur.
 * @param dh - Déplacement sur la hauteur.
 */
void d3v_camera_translate(struct camera *c, double dw, double dh)
{
    vec3 t = {
	dw * c->right.x + dh * c->up.x,
	dw * c->right.y + dh * c->up.y,
	dw * c->right.z + dh * c->up.z
    };
    c->look.x += t.x;
    c->look.y += t.y;
    c->look.z += t.z;
}

/**
 * Augmente ou réduit la distance de la camera.
 * @param c - La camera.
 * @param d - Distance à ajouter.
 */
void d3v_camera_add_distance(struct camera *c, double d)
{
    if (c->ortho) {
	glMatrixMode(GL_PROJECTION);
	if (d > 0)
	    c->orthosize *= 0.9;
	else 
	    c->orthosize *= 1.1;
    } else {
	c->dis = (c->dis + d <= 0.01) ? 0.0 : c->dis + d;
    }
}

/**
 * Change la rotation de la camera
 * @param c - La camera.
 * @param ar - angle par rapport au plan xz.
 * @param ay - angle par rapport à z.
 */
void d3v_camera_set_rotate(struct camera *c, int ar, int ay)
{
    c->ar = degree_to_radian(ar);
    c->ay = degree_to_radian(ay);
}

/**
 * Augmente ou réduit la rotation de la camera.
 * @param c - La camera.
 * @param ar - angle par rapport au plan xz.
 * @param ay - angle par rapport à z.
 */
void d3v_camera_rotate(struct camera *c, int ar, int ay)
{
    c->ar += degree_to_radian(ar);
    c->ay += degree_to_radian(ay);
}

/**
 * Met à jour la camera.
 * @param c - La camera.
 */
void d3v_camera_update(struct camera *c)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (c->ortho) {
	glOrtho(-10., 10., -10., 10., -100., 100.);
	glScaled(c->orthosize, c->orthosize, c->orthosize);
    } else {
        gluPerspective(30.0, 1.0, 0.1, 100.0);
    }
   
    camera_do_rotate(c, c->ar, c->ay);
    glMatrixMode(GL_MODELVIEW);
    gluLookAt(c->look.x + c->dis * c->rpos.x,
	      c->look.y + c->dis * c->rpos.y,
	      c->look.z + c->dis * c->rpos.z,
	      c->look.x, c->look.y, c->look.z,
	      c->up.x, c->up.y, c->up.z);
}
