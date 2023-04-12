#ifndef D3V_CAMERA_H
#define D3V_CAMERA_H

#include <utils/vec.h>

struct camera;

struct camera *d3v_camera_create(vec3 look, double dis, int ar, int ay,
				 int ortho);
void d3v_camera_switch_ortho(struct camera *c);
void d3v_camera_free(struct camera *c);

// reglage camera
void d3v_camera_set_look(struct camera *c, vec3* look);
void d3v_camera_set_distance(struct camera *c, double d);
void d3v_camera_set_rotate(struct camera *c, int ar, int ay);

// changement camera
void d3v_camera_translate(struct camera *c, double dx, double dy);
void d3v_camera_add_distance(struct camera *c, double d);
void d3v_camera_rotate(struct camera *c, int ar, int ay);

// "affichage"
void d3v_camera_update(struct camera *c);

#endif // D3V_CAM_H
