#ifndef PENGUINS_MATH_H
#define PENGUINS_MATH_H

#include <stdint.h>
#include <utils/vec.h>

void vec3_normalize(vec3 *v);

int radian_to_degree(double radian);
double degree_to_radian(int degree);

double angle_rotation_pingouin(const vec3 *src, const vec3 *dst);

double *rotation_matrix(vec3 ax, double angle);
void matrix_multiply(vec3 *P, double mat[9]);

uint64_t powi(uint64_t a, uint64_t p);

void rand_seed(void);
int randomint(int max);

double math_euclidian_distance(const vec3 *v1, const vec3 *v2);

#endif // PENGUINS_MATH_H
