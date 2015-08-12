#ifndef MATH_H
#define MATH_H

#include <utils/vec.h>

void vec3_normalize(vec3 *v);

int radian_to_degree(double radian);
double degree_to_radian(int degree);

double angle_rotation_pingouin(const vec3 *src, const vec3 *dst);

double *rotation_matrix(vec3 ax, double angle);
void matrix_multiply(vec3 *P, double mat[9]);

int powi(int a, int p);

void rand_seed(void);
int randomint(int max);

#endif //MATH_H
