/**
 * @file penrose.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "utils/fibonacci.h"

#include "./penrose.h"

#define LAYER 4
#define PHI 1.6180339887
#define EPSILON 0.000001
#define ABSC 0.363271264
#define ORD 1.53884176859
#define ANGLE 36
#define SQRT_5 2.2360679775

/**
 * Description d'un point.
 */
struct point {
    float x, y, z;
};

/**
 * Description d'un triangle.
 */
struct triangle {
    int a, b, c;
    int angle;
};

/**
 * Description d'un losange.
 */
struct lozenge {
    int point[4];   // ID de point dans le buffer de point
    int type;       // 0 = obtuse, 1 = acute
    int direction[4]; // ID de losange dans la direction donnée en indice
    int nb_direction; // 0 a 3 compris
    int angle;  // orientation de la tuile en degrés
};

/**
 * Decription d'un penrose.
 */
static struct {
    int step;
    int surface;

    struct point *point_buffer;
    int point_buffer_size;
    int nb_point;

    struct triangle *triangle_buffer;
    int triangle_buffer_size;
    int nb_triangle;

    struct lozenge *lozenge_buffer;
    int nb_lozenge;
} p;

/**
 * Obtenir le minimum de deux entiers.
 */
static inline int min(int a, int b)
{
    return a < b ? a : b;
}

/**
 * Calcule le nombre de récursions nécessaires à l'algorithme de génération de
 * penrose, le nombre maximal de losanges que peut contenir cette surface
 * et les stocke dans la structure penrose.
 * @param nb_lozenge - Nombre de losange.
 * @param dimension - Dimension de la carte : 1D, 2D, 3D, ...
 */
static void penrose_set_step_surface(int nb_lozenge, int dimension)
{
    struct fibonacci *f = fibonacci_init();
    int i = 0;
    int surface;
    int max_surface = ceil(nb_lozenge / pow(LAYER, dimension - 2.));

    do {
	i++;
	surface = (fibonacci_get(f, 2 * i + 1) - fibonacci_get(f, i + 2)) / 2;
    } while (surface < max_surface);

    p.step = i;
    p.surface = surface;
    fibonacci_free(f);
}

/**
 * Ajoute un point dans la structure penrose, s'il n'existe pas déjà.
 * @param x - Coordonnée x du point à ajouter.
 * @param y - Coordonnée y du point à ajouter.
 * @param z - Coordonnée z du point à ajouter.
 * @return int - Position du point ajouté dans le buffer de point.
 */
static int penrose_add_point(float x, float y, float z)
{
    for (int i = 0; i < p.nb_point; i++) {
	if (fabsf(x - p.point_buffer[i].x) < EPSILON &&
	    fabsf(y - p.point_buffer[i].y) < EPSILON &&
	    fabsf(z - p.point_buffer[i].z) < EPSILON)
	    return i;
    }

    if(p.nb_point >= p.point_buffer_size){
	p.point_buffer_size *= 2;
	p.point_buffer = realloc(p.point_buffer,
				  p.point_buffer_size * sizeof(struct point));
    }
    struct point O = { x, y, z };

    p.point_buffer[p.nb_point] = O;
    return p.nb_point++;
}

/**
 * Calcule le point M du segment [AB] tel que AM = A + (1 / phi) AB
 * @param a - Identifiant du première point.
 * @param b - Identifiant du deuxième point.
 * @return int - Position du point ajouté dans le buffer de point.
 */
static int penrose_add_aligned_point(int a, int b)
{
    struct point A = p.point_buffer[a];
    struct point B = p.point_buffer[b];

    return penrose_add_point(A.x + (B.x - A.x) / PHI,
			     A.y + (B.y - A.y) / PHI, A.z);
}

/**
 * Suppression d'un triangle du penrose.
 * @param triangle - Identifiant du triangle à supprimer.
 */
static void penrose_remove_triangle(int triangle)
{
    p.nb_triangle--;
    p.triangle_buffer[triangle] = p.triangle_buffer[p.nb_triangle];
}

/**
 * Calcule si le losange A a une arête en commum avec le losange B.
 * @param a - Premier losange.
 * @param b - Deuxième losange.
 * @return int - Retourne 1 si les losanges A et B ont une arête en commun, 0 sinon.
 */
static int penrose_has_common_edge(struct lozenge *a, struct lozenge *b)
{
    int match = 0;
    for (int i = 0; i < 4 && match < 2; i++)
	for (int j = 0; j < 4 && match < 2; j++)
	    if (a->point[i] == b->point[j])
		match++;
    return match == 2;
}

/**
 * Calcule si le losange A a un sommet en commum avec le losange B.
 * @param a - Premier losange.
 * @param b - Deuxième losange.
 * @return int - Retourne 1 si les losanges A et B ont un sommet en commun, 0 sinon.
 */
static int penrose_has_common_point(struct lozenge *a, struct lozenge *b)
{
    for (int i = 0; i < 4; i++)
	for (int j = 0; j < 4; j++)
	    if (a->point[i] == b->point[j])
	        return 1;
    return 0;
}

/**
 * Ajouter un losange au penrose.
 * @param a - Premier point.
 * @param b - Deuxième point.
 * @param c - Troisième point.
 * @param d - Quatrième point.
 * @param type - Type de losange. 0 = obtus, 1 = aigus.
 * @param angle - Orientation du losange.
 * @return int - Identifiant du losange.
 */
static int penrose_add_lozenge(int a, int b, int c, int d, int type, int angle)
{
    struct lozenge *m = &p.lozenge_buffer[p.nb_lozenge];

    m->point[0] = a;
    m->point[1] = b;
    m->point[2] = c;
    m->point[3] = d;

    m->nb_direction = 0;
    m->type = type;
    m->angle = angle;

    for (int i = 0; i < p.nb_lozenge; i++) {
	struct lozenge *n = &p.lozenge_buffer[i];
	if (penrose_has_common_edge(m, n)) {
	    m->direction[m->nb_direction] = i;
	    n->direction[n->nb_direction] = p.nb_lozenge;
	    m->nb_direction++;
	    n->nb_direction++;
	}
    }

    return p.nb_lozenge++;
}

/**
 * Ajouter un triangle au penrose.
 * @param a - Premier point.
 * @param b - Deuxième point.
 * @param c - Troisième point.
 * @param type - Type de losange. 0 = obtus, 1 = aigus.
 * @param angle - Orientation du losange.
 * @param nb_lozenge - Nombre de losanges. (modifié)
 * @return int - Position du triangle ajouté dans le buffer de triangles.
 */
static int penrose_add_triangle(int a, int b, int c, int type,
				int angle, int *nb_lozenge)
{
    if(p.nb_triangle >= p.triangle_buffer_size){
	p.triangle_buffer_size *= 2;
	p.triangle_buffer = realloc(p.triangle_buffer,
				     p.triangle_buffer_size *
				     sizeof(struct triangle));
    }

    for (int i = 0; i < p.nb_triangle; i++) {
	if (b == p.triangle_buffer[i].b && c == p.triangle_buffer[i].c) {
	    if(p.triangle_buffer[i].angle  % 180 != angle % 180)
		printf("%d %d\n", p.triangle_buffer[i].angle, angle);
	    if (type < 2)
		penrose_add_lozenge(a, b, p.triangle_buffer[i].a, c,
				    type % 2, angle);
	    else
		penrose_add_lozenge(p.triangle_buffer[i].a, b, a, c,
				    type % 2, angle);
	    penrose_remove_triangle(i);
	    *nb_lozenge = *nb_lozenge - 1;
	    return -1;
	}
    }

    p.triangle_buffer[p.nb_triangle].a = a;
    p.triangle_buffer[p.nb_triangle].b = b;
    p.triangle_buffer[p.nb_triangle].c = c;
    p.triangle_buffer[p.nb_triangle].angle = angle;
    return p.nb_triangle++;
}

/**
 * Génère le penrose.
 * @param a - Premier point.
 * @param b - Deuxième point.
 * @param c - Troisième point.
 * @param type - Type de losange. 0 = obtus, 1 = aigu.
 * @param angle - Orientation du losange.
 * @param step - Nombre d'étapes.
 * @param nb_lozenge - Nombre de losanges. (modifié)
 */
static void penrose_compute_penrose(int a, int b, int c, int type, int angle,
	     int step, int *nb_lozenge)
{
    if (*nb_lozenge > 0) {
	if (step <= 0)
	    penrose_add_triangle(a, b, c, type, angle, nb_lozenge);
	else {
	    int d, e;
	    switch (type) {
	    case 0:
		d = penrose_add_aligned_point(c, a);
		e = penrose_add_aligned_point(c, b);

		penrose_compute_penrose(d, c, e, 2, (angle + 5 * ANGLE) % 360,
					step - 1, nb_lozenge);
		penrose_compute_penrose(e, a, b, 0, (angle + 6 * ANGLE) % 360,
					step - 1, nb_lozenge);
		penrose_compute_penrose(e, a, d, 3, (angle + 9 * ANGLE) % 360,
					step - 1, nb_lozenge);
		break;
	    case 1:
		d = penrose_add_aligned_point(a, c);

		penrose_compute_penrose(d, a, b, 0, (angle + 7 * ANGLE) % 360,
					step - 1, nb_lozenge);
		penrose_compute_penrose(b, c, d, 1, (angle + 3 * ANGLE) % 360,
					step - 1, nb_lozenge);
		break;
	    case 2:
		d = penrose_add_aligned_point(c, a);
		e = penrose_add_aligned_point(c, b);

		penrose_compute_penrose(d, c, e, 0, (angle + 5 * ANGLE) % 360,
					step - 1, nb_lozenge);
		penrose_compute_penrose(e, a, b, 2, (angle + 4 * ANGLE) % 360,
					step - 1, nb_lozenge);
		penrose_compute_penrose(e, a, d, 1, (angle + 1 * ANGLE) % 360,
					step - 1, nb_lozenge);
		break;
	    case 3:
		d = penrose_add_aligned_point(a, c);

		penrose_compute_penrose(d, a, b, 2, (angle + 3 * ANGLE) % 360,
					step - 1, nb_lozenge);
		penrose_compute_penrose(b, c, d, 3, (angle + 7 * ANGLE) % 360,
					step - 1, nb_lozenge);
		break;
	    }
	}
    }
}

/**
 * Initialisation du penrose.
 * @param nb_lozenge - Nombre de losange.
 * @param dimension - Dimension de la carte : 1D, 2D, 3D, ...
 */
void penrose_init(int nb_lozenge, int dimension)
{
    p.point_buffer_size = 4;
    p.triangle_buffer_size = 4;
    p.point_buffer = malloc(4 * sizeof(struct point));
    p.triangle_buffer = malloc(4 * sizeof(struct triangle));
    p.lozenge_buffer = malloc(nb_lozenge * sizeof(struct lozenge));
    p.nb_point = 0;
    p.nb_triangle = 0;
    p.nb_lozenge = 0;

    penrose_set_step_surface(nb_lozenge, dimension);
    int step = p.step;
    int surface = p.surface;
    float scale = pow(PHI, step) / SQRT_5;
    float z = 0.;

    while (nb_lozenge > 0) {
	int nb_lozenge_per_surface = min(nb_lozenge, surface);
	int a = penrose_add_point(scale * 0.5, scale * ORD, z);
	int b = penrose_add_point(0., 0., z);
	int c = penrose_add_point(scale, 0., z);

	penrose_compute_penrose(a, b, c, 1, 0, step, &nb_lozenge_per_surface);
	nb_lozenge -= surface;
	z += 1.;
    }
}

/**
 * Obtenir le layer.
 * @return int - Le layer.
 */
int penrose_get_layer(void)
{
    return LAYER;
}

/**
 * Obtenir le nombre maximal de losanges que peut contenir une surface de penrose.
 * int - Nombre maximal de losanges par surface.
 */
int penrose_get_surface(void)
{
    return p.surface;
}

/**
 * Obtenir l'orientation d'un losange.
 * @param id - Identifiant du losange.
 * @return int - Orientation du losange en degré.
 */
int penrose_get_angle(int id)
{
    if (id < 0 || id >= p.nb_lozenge) {
	printf("penrose_get_angle: Error, invalid id\n");
	exit(EXIT_FAILURE);
    }

    return p.lozenge_buffer[id].angle;
}

/**
 * Obtenir le voisin d'un losange selon une direction.
 * @param id - Identifiant du losange.
 * @param direction - Direction à explorer.
 * @return int - Identifiant du voisin ou -1 si non trouvé.
 */
int penrose_get_neighbor(int id, int direction)
{
    if (id < 0 || id >= p.nb_lozenge) {
	printf("penrose_get_neighbor: Error, invalid id\n");
	exit(EXIT_FAILURE);
    }
    struct lozenge *l = &p.lozenge_buffer[id];
    if (direction >= 0 && direction < l->nb_direction)
	return l->direction[direction];
    return -1;
}

/**
 * Obtenir l'identifiant de destination.
 * @param id - Identifiant du losange de départ.
 * @param direction - Direction du mouvement. Le mappeur peut se permettre
 * de changer sa valeur pour indiquer au serveur quelle est la prochaine
 * direction à prendre pour avoir  un déplacement en ligne droite.
 * @return int - Identifiant de destination, -1 si pas de destination.
 */
int penrose_get_id_from_move(int id, int *direction)
{
    if (id < 0 || id >= p.nb_lozenge)
	return -1;

    struct lozenge *origin = &p.lozenge_buffer[id];
    if (*direction < 0 || *direction >= origin->nb_direction)
	return -1;

    int id_dest = origin->direction[*direction];
    struct lozenge *dest = &p.lozenge_buffer[id_dest];
    int found = -1;
    for (int i = 0; i < dest->nb_direction; i++) {
	struct lozenge *neighbor = &p.lozenge_buffer[dest->direction[i]];
	if (!penrose_has_common_point(origin, neighbor)) {
	    found = i;
	    break;
	}
    }
    *direction = found;
    return id_dest;
}

/**
 * Obtenir le type du losange.
 * @param id - Identifiant du losange.
 * @return int - Type du losange : 0 obtus, 1 aigu.
 */
int penrose_get_type(int id)
{
    if (id < 0 || id >= p.nb_lozenge) {
	printf("penrose_get_type: Error, invalid id\n");
	exit(EXIT_FAILURE);
    }
    return p.lozenge_buffer[id].type;
}

/**
 * Obtenir les coordonnées d'un losange.
 * @param id - Identifiant du losange.
 * @param coord - Coordonnées à remplir.
 */
void penrose_get_coordinates_from_id(int id, float coord[3])
{
    if (id < 0 || id >= p.nb_lozenge) {
	printf("penrose_get_coordinates_from_id: Error, invalid id\n");
	exit(EXIT_FAILURE);
    }
    struct point *A = &p.point_buffer[p.lozenge_buffer[id].point[0]];
    struct point *B = &p.point_buffer[p.lozenge_buffer[id].point[2]];

    coord[0] = (A->x + B->x) / 2.;
    coord[1] = (A->y + B->y) / 2.;
    coord[2] = A->z;
}

/**
 * Libération de la mémoire occupée par le penrose.
 */
void penrose_free(void)
{
    free(p.point_buffer);
    free(p.triangle_buffer);
    free(p.lozenge_buffer);
    memset(&p, 0, sizeof(p));
}
