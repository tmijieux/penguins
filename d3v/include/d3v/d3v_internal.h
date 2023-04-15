#ifndef D3V_INTERNAL_H
#define D3V_INTERNAL_H

#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include <utils/symbol_visibility.h>


extern int _d3v_binary_dir __internal;
extern struct scene scene  __internal;
extern GLFWwindow *window;

void d3v_key(int key, int x, int y) __internal;
void d3v_button(int button, int state, int x, int y) __internal;
void d3v_mouse_motion(int x, int y) __internal;
void d3v_reshape(int w, int h) __internal;

void d3v_scene_draw() __internal;


#endif //D3V_INTERNAL_H
