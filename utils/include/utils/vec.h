#ifndef PENGUINS_VEC_H
#define PENGUINS_VEC_H

typedef struct vec2 {
    float x, y;
} vec2;

typedef struct vec3 {
    float x, y, z;
} vec3;

typedef struct vec4 {
    float x, y, z, w;
} vec4;


typedef struct mat3 {
    float m[9];
} mat3;

typedef struct mat4 {
    float m[16];
} mat4;

typedef struct mat4d {
    double m[16];
} mat4d;


#endif // PENGUINS_VEC_H
