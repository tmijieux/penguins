#ifndef D3_H
#define D3_H

#include <utils/vec.h>
#include <d3v/user_callback.h>

int d3v_init(int);
int d3v_start(vec3 *pos);
int d3v_exit(void);

#endif //D3_H
