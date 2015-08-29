#ifndef D3V_INTERNAL_H
#define D3V_INTERNAL_H

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/gl.h>
#include <GL/glx.h>

#include <utils/so.h>

extern struct scene scene __so_local;
extern Display *display __so_local;
extern Window win __so_local;
extern GLXContext ctx __so_local;
extern Colormap cmap __so_local;

void d3v_key(unsigned char key, int x, int y) __so_local;
void d3v_special_input(int key, int x, int y) __so_local;
void d3v_button(int button, int state, int x, int y) __so_local;
void d3v_mouse_motion(int x, int y) __so_local;
void d3v_reshape(int w, int h) __so_local;

void d3v_scene_draw(void) __so_local;


#endif //D3V_INTERNAL_H
