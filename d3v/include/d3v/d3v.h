#ifndef PENGUINS_D3V_H
#define PENGUINS_D3V_H

#include <GL/gl.h>
#include <GL/glu.h>

#include <utils/vec.h>
#include <utils/symbol_visibility.h>

#include <d3v/user_callback.h>


#define MY_GL_ERROR()  printf("error: %s\n\n", gluErrorString(glGetError()))
#define HANDLE_GL_ERROR(x) do {                                         \
        (x);                                                            \
        int __err;                                                      \
        while ((__err = glGetError()) != GL_NO_ERROR) {                 \
            fprintf(stderr, "GL Error happend code=0x%x at %s : %d\n", __err, __FILE__, __LINE__); \
            MY_GL_ERROR();                                              \
        }                                                               \
    } while(0)



void d3v_init_asset_path(int v) __export;

int d3v_module_init(int object_count_hint) __export ;
int d3v_module_exit(void) __export ;
int d3v_main_loop(void) __export ;
void d3v_set_initial_camera_position(const vec3 *pos);

void d3v_request_animation_frame(void);
void d3v_request_exit_from_main_loop(void);

void d3v_draw_text(char c, vec3 pos);

#endif // PENGUINS_D3V_H
