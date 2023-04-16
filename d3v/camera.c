/**
 * @file camera.c
 */
#ifdef _WIN32
#include <Windows.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "penguins_opengl.h"

#include "utils/math.h"
#include "d3v/camera.h"
#include "d3v/d3v.h"

struct camera {

    vec3 look; // Point observé
    vec3 rpos; // Position dans la base (look, x, y ,z) (direction)
    double distance; // Distance par rapport au point observé

    vec3 up; // direction du haut
    vec3 right; // direction de la droite

    double angleX; // "latitude" angle around X axis
    double angleY; // "longitude" angle around Y axis

    int ortho;
    double orthosize;

    int dirty; // camera moved/changed, view and projection matrix must be recomputed

    mat4 proj;// stored in column major format
    mat4 view;// stored in row major format (transpose=GL_TRUE)
};


static void camera_compute_axes(camera_t *c, double angleX, double angleY)
{
    c->right = (vec3) {1., 0., 0.};
    c->up    = (vec3) {0., 1., 0.};
    c->rpos  = (vec3) {0., 0., 1.};

    mat4 rot;
    // first, rotate around X axis to elevate our head around the ground
    // ( up from (z,x) plan)
    make_rotation_matrix(c->right, angleX, &rot);

    // rotate two other axes of the camera to follow the first rotation
    mat4_multiply3(&c->up, &rot);
    mat4_multiply3(&c->rpos, &rot);

    // rotate around Y axis
    make_rotation_matrix((vec3) {0., 1., 0.}, angleY, &rot);
    mat4_multiply3(&c->up, &rot);
    mat4_multiply3(&c->rpos, &rot);
    mat4_multiply3(&c->right, &rot);

    c->dirty = 1;
}


/**
 * Création de la camera.
 * @return camera_t * - La camera.
 * @param look - Position où regarde la camera.
 * @param distance - Distance de la camera.
 * @param ar - angle par rapport au plan xz.
 * @param ay - angle par rapport à z.
 * @param ortho - 1 si le mode orthogonal est activé.
 */
camera_t *
d3v_camera_create(vec3 look, double distance, int angleX, int angleY, int ortho)
{
    camera_t *c = calloc(sizeof(*c), 1);
    c->ortho = ortho;
    c->orthosize = 1.;
    c->angleX = degree_to_radian(angleX);
    c->angleY = degree_to_radian(angleY);
    c->look = look;
    c->distance = distance;
    c->dirty = 1;
    camera_compute_axes(c, c->angleX, c->angleY);
    return c;
}

/**
 * Change le mode de projection de la camera.
 * @param c - La camera.
 */
void d3v_camera_switch_ortho(camera_t *c)
{
    c->ortho = !c->ortho;
    if (!c->ortho) {
	double d = 1. / c->orthosize;
	/* glMatrixMode(GL_MODELVIEW); */
	/* glScaled(d, d, d); */
	c->orthosize = 1.;
    }
    c->dirty = 1;
}

/**
 * Libère la mémoire occupé par la camera.
 * @param c - La camera.
 */
void d3v_camera_free(camera_t *c)
{
    memset(c, 0, sizeof *c);
    free(c);
}

/**
 * Change la position où regarde la camera.
 * @param c - La camera.
 * @param look - Nouvelle position.
 */
void d3v_camera_set_look(camera_t *c, const vec3 *look)
{
    c->look = *look;
    c->dirty = 1;
}

/**
 * Change la distance de la camera.
 * @param c - La camera.
 * @param d - Distance de la camera.
 */
void d3v_camera_set_distance(camera_t *c, double d)
{
    c->distance = d;
    c->dirty = 1;
}

/**
 * Déplacement de la camera
 * @param c - La camera.
 * @param dw - Déplacement sur la largeur.
 * @param dh - Déplacement sur la hauteur.
 */
void d3v_camera_translate(camera_t *c, double dw, double dh)
{
    vec3 t = {
	dw * c->right.x + dh * c->up.x,
	dw * c->right.y + dh * c->up.y,
	dw * c->right.z + dh * c->up.z
    };
    c->look.x += t.x;
    c->look.y += t.y;
    c->look.z += t.z;
    c->dirty = 1;
}

/**
 * Augmente ou réduit la distance de la camera.
 * @param c - La camera.
 * @param d - Distance à ajouter.
 */
void d3v_camera_add_distance(camera_t *c, double d)
{
    if (c->ortho) {
	if (d > 0) {
	    c->orthosize *= 0.9;
        } else {
	    c->orthosize *= 1.1;
        }
    } else {
	c->distance = (c->distance + d <= 0.01) ? 0.0 : c->distance + d;
    }
    c->dirty = 1;
}

/**
 * Change la rotation de la camera
 * @param c - La camera.
 * @param ar - angle par rapport au plan xz.
 * @param ay - angle par rapport à z.
 */
void d3v_camera_set_rotate(camera_t *c, int angleX, int angleY)
{
    c->angleX = degree_to_radian(angleX);
    c->angleY = degree_to_radian(angleY);
    c->dirty = 1;
}

/**
 * Augmente ou réduit la rotation de la camera.
 * @param c - La camera.
 * @param ar - angle par rapport au plan xz.
 * @param ay - angle par rapport à z.
 */
void d3v_camera_rotate(camera_t *c, int angleX, int angleY)
{
    c->angleX += degree_to_radian(angleX);
    c->angleY += degree_to_radian(angleY);
    c->dirty = 1;
}


/**
 * Met à jour la camera.
 * @param c - La camera.
 */
int d3v_camera_update(camera_t *c)
{
    if (!c->dirty) {
        return 0;
    }

    // compute PROJ matrix:
    if (c->ortho) {
	/* glOrtho(-10., 10., -10., 10., -100., 100.); */
	/* glScaled(c->orthosize, c->orthosize, c->orthosize); */

        // FIXME ortho
        make_proj_perspective(30.0, 1.0, 0.1, 100.0, &c->proj);
    } else {
        make_proj_perspective(30.0, 1.0, 0.1, 100.0, &c->proj);
    }

    // compute VIEW matrix:
    camera_compute_axes(c, c->angleX, c->angleY);
    vec3 eye;
    eye.x = c->look.x + c->distance * c->rpos.x;
    eye.y = c->look.y + c->distance * c->rpos.y;
    eye.z = c->look.z + c->distance * c->rpos.z;

    make_view_look_at(c->look, eye, c->up, &c->view);

    c->dirty = 0;
    return 1;
}

void d3v_camera_draw(camera_t *c, int shader_program)
{
    glUseProgram(shader_program);

    if (shader_program != 0) {
        uint32_t loc = glGetUniformLocation(shader_program, "proj");
        if (loc != -1) {
            glUniformMatrix4fv(loc, 1, GL_FALSE, c->proj.m);
        }
        loc = glGetUniformLocation(shader_program, "view");
        if (loc != -1) {
            glUniformMatrix4fv(loc, 1, GL_TRUE, c->view.m);
        }
    }
}

static void matrix_to_double(const mat4 *in, mat4d *out)
{
    for (int i = 0; i < 16; ++i) {
        out->m[i] = (double)in->m[i];
    }
}

void d3v_camera_get_view_matrix_d(camera_t *c, mat4d *view)
{
    matrix_to_double(&c->view, view);
    mat4d_transpose(view);
}

void d3v_camera_get_proj_matrix_d(camera_t *c, mat4d *proj)
{
    matrix_to_double(&c->proj, proj);
}
