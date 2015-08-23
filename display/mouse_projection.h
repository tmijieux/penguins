#ifndef MOUSE_PROJECTION_H
#define MOUSE_PROJECTION_H

#include <display/mouseclick.h>

void mc_cond_signal(void);
void mc_set_pos(const vec3 *pos);
int mc_mouse_projection(vec3 *pos, int x, int y);
    
#endif //MOUSE_PROJECTION_H
