#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include <math.h>
#include <utils/vec.h>
#include <utils/math.h>

#define EPSILON 0.01

/**
 * @file
 */

void test_vec3_normalize(void)
{
    vec3 vec = {4, 5, 3};
    vec3_normalize(&vec);
    assert(abs(vec.x*vec.x + vec.y*vec.y + vec.z*vec.z - 1.) < EPSILON);
}


void test_radian_to_degree(void)
{
    assert(abs(radian_to_degree(3 * M_PI / 4) - 135) < 3.);
    assert(abs(radian_to_degree(M_PI/2) - 90) < 3.);
}

void test_degree_to_radian(void)
{
    assert(abs(degree_to_radian(135) - (3 * M_PI / 4)) < EPSILON);
    assert(abs(degree_to_radian(90) - (M_PI / 2)) < EPSILON);
}

void test_matrix_multiply(void)
{
    vec3 vec = {1, 2, 3};
    double mat[9] = {2, 4, 5, 1, 2, 2, 5, 6, 3};
    matrix_multiply(&vec, mat);
    assert((abs(vec.x - 25) < EPSILON) 
	   && (abs(vec.y - 11) < EPSILON) 
	   && (abs(vec.z - 26) < EPSILON));
}

int main(int argc, char **argv)
{
    test_vec3_normalize();
    test_radian_to_degree();
    test_degree_to_radian();
    test_matrix_multiply();
    return EXIT_SUCCESS;
    
}
