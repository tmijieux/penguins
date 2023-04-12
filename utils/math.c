/**
 * @file math.c
 * various math stuff
 */

#define __USE_XOPEN 1

#include <stdlib.h>
#include <math.h>
#include <stdint.h>

#include <string.h>
#include <time.h>

#include <utils/math.h>
#include <utils/vec.h>

#define SQUARE(x) ((x) * (x))

/**
 * Normaliser un vecteur.
 * @param v - Vecteur à normaliser (modifié).
 */
void vec3_normalize(vec3 *v)
{
	double n = sqrt(v->x*v->x+v->y*v->y+v->z*v->z);
	v->x /= n;  v->y /= n;  v->z /= n;
}

/**
 * Convertion radian -> degrée.
 * @param radian - Angle en radian.
 * @return int - Angle en degrée.
 */
int radian_to_degree(double radian)
{
	return (int) (radian * 180. / M_PI);
}

/**
 * Convertion degrée -> radian.
 * @param degree - Angle en degrée.
 * @return double - Angle en radian.
 */
double degree_to_radian(int degree)
{
	return (double) degree * M_PI / 180.;
}

/**
 * Obtenir l'angle en degrée décrit par un vecteur.
 * @param src - Source du vecteur.
 * @param dst - Destination du vecteur.
 * @return double - Angle en degrée.
 */
double angle_rotation_pingouin(const vec3 *src, const vec3 *dst)
{
	vec3 a = { dst->x-src->x, 0., dst->z-src->z };
	if (a.x == 0. && a.z == 0.)
		return 400.;
	vec3_normalize(&a);  double s = -a.x;
	return radian_to_degree( (s < 0 ? 1. : -1.) * acos(a.z));
}

/**
 * Obtenir la matrice de rotation décrite par une position et un angle.
 * @param ax - Position d'un point.
 * @param angle - Angle de rotation.
 * @return double * - Matrice de rotation.
 */
double *rotation_matrix(vec3 ax, double angle)
{
	double co = cos(angle), co2 = 1.-co, si = sin(angle);
	double xx = ax.x*ax.x, yy = ax.y*ax.y, zz = ax.z*ax.z;
	double xy = ax.x*ax.y, xz = ax.x*ax.z, yz = ax.y*ax.z; 
	double r[9] = {
		co+xx*co2, xy*co2-ax.z*si, xz*co2+ax.y*si,
		xy*co2+ax.z*si, co+yy*co2, yz*co2-ax.x*si,
		xz*co2-ax.y*si, yz*co2+ax.x*si, co+zz*co2
	};
	double *rot = malloc(9 * sizeof(*rot));
	memcpy(rot, r, 9 * sizeof(*rot));
	return rot;
}

/**
 * Multiplier un vec3 par une matrice.
 * @param P - vec3 à multiplier (modifié).
 * @param mat - Matrice.
 */
void matrix_multiply(vec3 *P, double mat[9])
{
	vec3 p = *P;
	P->x = mat[0]*p.x+mat[1]*p.y+mat[2]*p.z;
	P->y = mat[3]*p.x+mat[4]*p.y+mat[5]*p.z;
	P->z = mat[6]*p.x+mat[7]*p.y+mat[8]*p.z;
}

/**
 * Calcule a à la puissance b.
 * @param a - Facteur.
 * @param b - Exposant.
 * @return int - a exposant b.
 */
uint64_t powi(uint64_t a, uint64_t p)
{
    uint64_t b = 1;
    while (p) {
	register uint64_t c = p%2;
	b *= a * c + (1 - c);
	a *= a;
	p >>= 1;
    }
    return b;
}

/**
 * entier aléatoire entre 0 et max;
 */
int randomint(int max)
{
    double a = (double)rand() / RAND_MAX;
    return (int)(a * max);
}

/**
 * seed the random system
 */
void rand_seed(void)
{
    static int once = 0;
    if (!once) {
	union upi {
	    union upi *pupi;
	    unsigned int u;
	} a;
	a.pupi = &a;
	srand(time(NULL) + a.u);
	once = 1;
    }
}


/**
 * norm 1 :: for 'square' sphere 
 */
double dnorm1(const vec3 *v1, const vec3 *v2)
{
    return
	fabs(v1->x - v2->x) +
	fabs(v1->y - v2->y) +
	fabs(v1->z - v2->z);
}

double math_euclidian_distance(const vec3 *v1, const vec3 *v2)
{
    return
	sqrt(
	    SQUARE(v1->x - v2->x) +
	    SQUARE(v1->y - v2->y) +
	    SQUARE(v1->z - v2->z)
	);
}
