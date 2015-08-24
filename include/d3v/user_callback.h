#ifndef USER_CALLBACK_H
#define USER_CALLBACK_H

void d3v_set_mouse_callback(
    void (*mouse_callback)(int,int,int,int));
void d3v_set_spe_input_callback(
    void (*spe_input_callback)(int,int,int));
void d3v_set_key_input_callback(
    void (*key_input_callback)(int,int,int));
void d3v_set_draw_callback(
    void (*draw_callback)(void));

#endif //USER_CALLBACK_H
