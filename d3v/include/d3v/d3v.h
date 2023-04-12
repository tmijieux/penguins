#ifndef PENGUINS_D3V_H
#define PENGUINS_D3V_H

#include <utils/vec.h>
#include <utils/symbol_visibility.h>

#include <d3v/user_callback.h>

extern int _d3v_binary_dir;

#define HANDLE_GL_ERROR(x) do {                                 \
        (x);                                                    \
        int __err;                                              \
        while ((__err = glGetError()) != GL_NO_ERROR) {        \
            fprintf(stderr, "GL Error happend code=0x%x at %s : %d\n", __err, __FILE__, __LINE__); \
        }                                                       \
    } while(0)

int d3v_init(int) __export ;
void d3v_init_asset_path(int v) __export;
int d3v_start(vec3 *pos) __export ;
int d3v_exit(void) __export ;

void d3v_post_redisplay(void);
void d3v_exit_main_loop(void);

#endif // PENGUINS_D3V_H
