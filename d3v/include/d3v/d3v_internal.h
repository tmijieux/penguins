#ifndef D3V_INTERNAL_H
#define D3V_INTERNAL_H

#include "penguins_opengl.h"

#include <utils/symbol_visibility.h>

extern struct scene scene  __internal;
extern GLFWwindow *window;

void d3v_key(int key, int scancode, int action, int mods, int x, int y) __internal;
void d3v_button(int button, int state, int mods, int x, int y) __internal;
void d3v_mouse_motion(int x, int y) __internal;
void d3v_reshape(int w, int h) __internal;
void d3v_scene_draw(void) __internal;

#endif // D3V_INTERNAL_H
