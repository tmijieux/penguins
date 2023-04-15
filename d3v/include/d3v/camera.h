#ifndef D3V_CAMERA_H
#define D3V_CAMERA_H

#include <utils/vec.h>

struct camera;
typedef struct camera camera_t;

camera_t *d3v_camera_create(vec3 look, double dis, int ar, int ay,
				 int ortho);
void d3v_camera_switch_ortho(camera_t *c);
void d3v_camera_free(camera_t *c);

// reglage camera
void d3v_camera_set_look(camera_t *c, const vec3* look);
void d3v_camera_set_distance(camera_t *c, double d);
void d3v_camera_set_rotate(camera_t *c, int ar, int ay);

// changement camera
void d3v_camera_translate(camera_t *c, double dx, double dy);
void d3v_camera_add_distance(camera_t *c, double d);
void d3v_camera_rotate(camera_t *c, int ar, int ay);

// "affichage"
int d3v_camera_update(camera_t *c);// return whether a draw is required

// must be called once per shader ???
void d3v_camera_draw(camera_t *c);

void d3v_camera_get_view_matrix_d(camera_t *c, mat4d *view);
void d3v_camera_get_proj_matrix_d(camera_t *c, mat4d *proj);

#endif // D3V_CAMERA_H
