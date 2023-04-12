#ifndef D3V_MOUSE_PROJECTION_H
#define D3V_MOUSE_PROJECTION_H

#include <utils/vec.h>

// may be used in the mouse event callback !!
// NEVER in any other place !!
int d3v_mouseproj(vec3 *pos, int x, int y);

#endif // D3V_MOUSE_PROJECTION_H
