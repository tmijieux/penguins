#ifndef D3V_USER_CALLBACK_H
#define D3V_USER_CALLBACK_H

typedef void (*mouse_callback_t)(int button, int state, int x,int y);
typedef void (*key_input_callback_t)(int key, int scancode, int mods, int x,int y);
typedef void (*draw_callback_t)(void);
typedef void (*exit_callback_t)(void);

void d3v_set_mouse_callback(mouse_callback_t mouse_callback);
void d3v_set_key_input_callback(key_input_callback_t key_input_callback);
void d3v_set_draw_callback(draw_callback_t draw_callback);
void d3v_set_exit_callback(exit_callback_t exit_callback);

#endif // D3V_USER_CALLBACK_H
