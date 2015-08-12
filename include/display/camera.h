#ifndef CAMERA_H
#define CAMERA_H

#include <utils/vec.h>

enum EVENTTYPE {
    UP = 0,
    DOWN = 1
};

struct camera;

struct camera *camera_create(vec3 look, double dis,
			     int ar, int ay,
			     int ortho);
void camera_switch_ortho(struct camera *c);
void camera_free(struct camera *c);

// reglage camera
void camera_set_look(struct camera *c, vec3 look);
void camera_set_distance(struct camera *c, double d);
void camera_set_rotate(struct camera *c, int ar, int ay);

// changement camera
void camera_translate(struct camera *c, double dx, double dy);
void camera_add_distance(struct camera *c, double d);
void camera_rotate(struct camera *c, int ar, int ay);

// "affichage"
void camera_update(struct camera *c);



#endif	//CAM_H
