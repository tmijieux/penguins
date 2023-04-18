/**
 * @file math.c
 * various math stuff
 */

#define __USE_XOPEN 1
#define _USE_MATH_DEFINES // required for windows
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

#include <string.h>
#include <time.h>

#include "utils/math.h"
#include "utils/vec.h"

#define SQUARE(x) ((x) * (x))

/**
 * Normaliser un vecteur.
 * @param v - Vecteur à normaliser (modifié).
 */
void vec3_normalize(vec3 *v)
{
    float n = sqrtf(v->x * v->x
                    + v->y * v->y
                    + v->z * v->z);
    v->x /= n;
    v->y /= n;
    v->z /= n;
}

void vec4_normalize(vec4 *v)
{
    float n = sqrtf(v->x * v->x
                    + v->y * v->y
                    + v->z * v->z);
    v->x /= n;
    v->y /= n;
    v->z /= n;
    v->w = 1;
}

vec3 vec3_cross(const vec3 *u, const vec3 *v)
{
    vec3 w;
    w.x = u->y * v->z - u->z * v->y;
    w.y = - (u->x * v->z) + (u->z * v->x);
    w.z = u->x * v->y - u->y * v->x;
    return w;
}


vec4 vec4_cross(const vec4 *u, const vec4 *v)
{
    vec4 w;
    w.x = u->y * v->z - u->z * v->y;
    w.y = - (u->x * v->z) + (u->z * v->x);
    w.z = u->x * v->y - u->y * v->x;
    w.w = 1;
    return w;
}

float vec3_dot(const vec3 *u, const vec3 *v)
{
    return  u->x * v->x
        + u->y * v->y
        + u->z * v->z;
}


double angle_normalize(double degree)
{
    while (degree < 0.0f) {
        degree += 360.0f;
    }
    while (degree >= 360.0f) {
        degree -= 360.0f;
    }
    return degree;
}



/**
 * Convertion radian -> degrée.
 * @param radian - Angle en radian.
 * @return int - Angle en degrée.
 */
double radian_to_degree(double radian)
{
    return (radian * 180. / M_PI);
}

/**
 * Convertion degrée -> radian.
 * @param degree - Angle en degrée.
 * @return double - Angle en radian.
 */
double degree_to_radian(double degree)
{
    return (double) degree * M_PI / 180.;
}

/**
 * Obtenir l'angle en degrée décrit par un vecteur.
 * @param src - Source du vecteur.
 * @param dst - Destination du vecteur.
 * @return double - Angle en degrée.
 */
float vec3_angle_zx(const vec3 *src, const vec3 *dst)
{
    float dx = dst->x - src->x;
    float dz = dst->z - src->z;

    if (fabs(dx) < 1e-4 && fabs(dz) < 1e-4) {
        return 0.0;
    }

    vec3 a = {dx, 0, dz};
    vec3_normalize(&a);
    double s = -a.x;
    return angle_normalize(radian_to_degree((s < 0 ? 1. : -1.) * acos(a.z)));
}

/**
 * Obtenir la matrice de rotation décrite par une position et un angle.
 * @param ax - Position d'un point.
 * @param angle - Angle de rotatio en radian
 * @return double * - Matrice de rotation.
 */
void make_rotation_matrix(vec3 ax, double angle, mat4 *out)
{
    float co = cos(angle), co2 = 1.-co, si = sin(angle);
    float xx = ax.x*ax.x, yy = ax.y*ax.y, zz = ax.z*ax.z;
    float xy = ax.x*ax.y, xz = ax.x*ax.z, yz = ax.y*ax.z;
    float r[16] = {
        co+xx*co2, xy*co2-ax.z*si, xz*co2+ax.y*si, 0,
        xy*co2+ax.z*si, co+yy*co2, yz*co2-ax.x*si, 0,
        xz*co2-ax.y*si, yz*co2+ax.x*si, co+zz*co2, 0,
        0, 0, 0, 1.0f
    };

    memcpy(out->m, r, sizeof(out->m));
}

void make_uniform_scale_matrix(float val, mat4 *out)
{
    vec3 v ={val,val,val};
    make_scale_matrix(v, out);
}

void make_scale_matrix(vec3 sc, mat4 *out)
{
    memset(out->m, 0, sizeof(out->m));
    out->m[0] = sc.x;
    out->m[5] = sc.y;
    out->m[10] = sc.z;
    out->m[15] = 1.0f;
}

void make_identity(mat4 *out)
{
    memset(out->m, 0, sizeof(out->m));
    out->m[0] = out->m[5] = out->m[10] = out->m[15] = 1.0f;
}

void make_translation_matrix(vec3 t, mat4 *out)
{
    make_identity(out);
    out->m[ 3] = t.x;
    out->m[ 7] = t.y;
    out->m[11] = t.z;
}

/**
 * Multiplier un vec3 par une matrice.
 * @param P - vec3 à multiplier (modifié).
 * @param mat - Matrice.
 */
void mat3_multiply(vec3 *P, const mat3 * mat)
{
    vec3 p = *P;
    const float *m = mat->m;

    P->x = m[0]*p.x+m[1]*p.y+m[2]*p.z;
    P->y = m[3]*p.x+m[4]*p.y+m[5]*p.z;
    P->z = m[6]*p.x+m[7]*p.y+m[8]*p.z;
}

/**
 * Multiplier un vec4 par une matrice.
 * @param P - vec4 à multiplier (modifié).
 * @param mat - Matrice.
 */
void mat4_multiply(vec4 *P, const mat4 * mat)
{
    vec4 p = *P;
    const float *m = mat->m;

    P->x = m[0] * p.x + m[1] * p.y + m[2] * p.z + m[3] * p.w;
    P->y = m[4] * p.x + m[5] * p.y + m[6] * p.z + m[7] * p.w;
    P->z = m[8] * p.x + m[9] * p.y + m[10] * p.z + m[11] * p.w;
    P->w = m[12] * p.x + m[13] * p.y + m[14] * p.z + m[15] * p.w;
}

void mat4_multiply3(vec3 *P, const mat4 *mat)
{
    vec3 p = *P;
    const float *m = mat->m;

    P->x = m[0] * p.x + m[1] * p.y + m[2] * p.z;
    P->y = m[4] * p.x + m[5] * p.y + m[6] * p.z;
    P->z = m[8] * p.x + m[9] * p.y + m[10] * p.z;
}

void mm_multiply(const mat4 *u, const mat4 *v, mat4 *w)
{
    memset(w->m, 0, sizeof(w->m));
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            for (int k = 0; k < 4; ++k) {
                w->m[i*4+j] += u->m[i*4+k] *  v->m[k*4+j];
            }
        }
    }
}

void mat4_print(const mat4 *u)
{
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            float v =u->m[i*4+j];
            printf("%.5f\t", v);
        }
        printf("\n");
    }
    printf("\n");
}

void mat4d_print(const mat4d *u)
{
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            double v =u->m[i*4+j];
            printf("%.5g\t", v);
        }
        printf("\n");
    }
    printf("\n");
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

void make_view_look_at(vec3 CENTER, vec3 EYE, vec3 UP, mat4 *out)
{
    vec3 F = CENTER;
    F.x -= EYE.x;
    F.y -= EYE.y;
    F.z -= EYE.z;

    vec3 f = F;
    vec3_normalize(&f);

    /* printf("pos=%.3f\t %.3f\t %.3f\t\n",pos.x, pos.y, pos.z); */
    /* printf("dir=%.3f\t %.3f\t %.3f\t\n",dir.x, dir.y, dir.z); */

    vec3_normalize(&UP);
    vec3 s = vec3_cross(&f, &UP);
    vec3_normalize(&s);
    vec3 u = vec3_cross(&s, &f);

    /* float m[16] =  { */
    /*     s.x, u.x, -f.x, 0.0, */
    /*     s.y, u.y, -f.y, 0.0, */
    /*     s.z, u.z, -f.z, 0.0, */
    /*     0.0, 0.0, 0.0,  1.0 */
    /* }; */

    float m[16] =  {
        s.x, s.y, s.z, 0.0,
        u.x, u.y, u.z, 0.0,
        -f.x, -f.y, -f.z, 0.0,
        0.0, 0.0, 0.0,  1.0
    };
    mat4 rot;
    memcpy(rot.m, m, sizeof(rot.m));

    vec3 t = {-EYE.x,-EYE.y,-EYE.z};
    mat4 trans;
    make_translation_matrix(t, &trans);

    mm_multiply(&rot, &trans, out);
}

void make_proj_orthogonal(float left, float right,
                          float bottom, float top,
                          float nearVal, float farVal,
                          mat4 *out)
{

    float dx = (right-left);
    float dy = (top-bottom);
    float dz = (farVal-nearVal);
    float tx = -(right+left)/dx;
    float ty = -(top+bottom)/dy;
    float tz = -(farVal+nearVal)/dz;

    float m[] = {
        2/dx, 0,    0,     tx,
        0,    2/dy, 0,     ty,
        0,    0,    -2/dz, tz,
        0,    0,    0,      1
    };
    memcpy(out->m, m, sizeof(out->m));
}

void make_proj_perspective(float fov, float aspect, float near, float far, mat4 *out)
{
    double D2R = M_PI / 180.0;
    double yScale = 1.0 / tan(D2R * fov / 2);
    double xScale = yScale / aspect;
    double nearmfar = near - far;
    float m[] = {
        xScale, 0, 0, 0,
        0, yScale, 0, 0,
        0, 0, (far + near) / nearmfar, -1,
        0, 0, 2*far*near / nearmfar, 0
    };
    memcpy(out->m, m, sizeof(out->m));
}


void mat4d_transpose(mat4d *inout)
{
    for (int i = 0; i < 4; ++i)  {
        for (int j = 0; j < i; ++j)  {
            double tmp = inout->m[i*4+j];
            inout->m[i*4+j] = inout->m[j*4+i];
            inout->m[j*4+i] = tmp;
        }
    }
}
