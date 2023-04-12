#ifndef D3V_INTERNAL_H
#define D3V_INTERNAL_H

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/gl.h>
#include <GL/glx.h>

#include <utils/symbol_visibility.h>

extern struct scene scene  __internal;
extern Display *display __internal;
extern Window win __internal;
extern GLXContext ctx __internal;
extern Colormap cmap __internal;

void d3v_key(unsigned char key, int x, int y) __internal;
void d3v_special_input(int key, int x, int y) __internal;
void d3v_button(int button, int state, int x, int y) __internal;
void d3v_mouse_motion(int x, int y) __internal;
void d3v_reshape(int w, int h) __internal;

void d3v_scene_draw(void) __internal;


#endif //D3V_INTERNAL_H
