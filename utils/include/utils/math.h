#ifndef PENGUINS_MATH_H
#define PENGUINS_MATH_H

#include <stdint.h>

#include "utils/vec.h"

void vec3_normalize(vec3 *v);
void vec4_normalize(vec4 *v);

vec4 vec4_cross(const vec4 *v1, const vec4 *v2);
float vec3_dot(const vec3 *v1, const vec3 *v2);

double radian_to_degree(double radian);
double degree_to_radian(double degree);

double angle_rotation_pingouin(const vec3 *src, const vec3 *dst);

void make_rotation_matrix(vec3 ax, double angle, mat4 *out);
void make_translation_matrix(vec3 t, mat4 *out);
void make_scale_matrix(vec3 sc, mat4 *out);
void make_uniform_scale_matrix(float val, mat4 *out);
void make_identity(mat4 *out);

void mat3_multiply(vec3 *P, const mat3 *mat);
void mat4_multiply(vec4 *P, const mat4 *mat);
void mat4_multiply3(vec3 *P, const mat4 *mat);

void mat4d_transpose(mat4d *inout);

void mm_multiply(const mat4 *u, const mat4 *v, mat4 *w);

void mat4_print(const mat4 *u);


uint64_t powi(uint64_t a, uint64_t p);

void rand_seed(void);
int randomint(int max);

double math_euclidian_distance(const vec3 *v1, const vec3 *v2);

void make_proj_perspective(float fov, float aspect, float near, float far, mat4 *out);
void make_proj_orthogonal(float left, float right,
                          float bottom, float top,
                          float nearVal, float farVal,
                          mat4 *out);

void make_view_look_at(vec3 CENTER, vec3 EYE, vec3 UP, mat4 *out);

void make_translation_matrix(vec3 t, mat4 *out);

#endif // PENGUINS_MATH_H
