#ifndef MOUSE_PROJECTION_H
#define MOUSE_PROJECTION_H

#include <display/mouseclick.h>

// may be used in the mouse event callback !!
// NEVER in any other place !!
int d3v_mouseproj(vec3 *pos, int x, int y);
    
#endif //MOUSE_PROJECTION_H
