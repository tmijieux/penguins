/**
 * @file model.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <stddef.h>

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>

#include <utils/vec.h>
#include <server/path.h>
#include <d3v/model.h>

#define LINE_SIZE 128

/**
 * Description d'un vertex.
 */
struct vertex {
    vec3 pos;
    vec3 normal;
    vec2 tex_coord;
};

/**
 * Description du model.
 */
struct model {
    struct vertex *buffer;
    unsigned int vcount;
    int uv, normal; // booleans
    GLuint vbo;
};

/**
 * Retourne le nombre de vertex d'une face décrit dans la ligne.
 * @param line - Chaine de charactères.
 * @return int - Nombre de vertex.
 */
static int model_wavefront_face_vertex_count(char *line)
{
    int c = 0;
    char *li = strdup(line);
    char *tok = strtok(li, " ");
    while (tok != NULL) {
	c++;
	tok = strtok(NULL, " ");
    }
    free(li);
    return c;
}

/**
 * Lecture du wavefront.
 * @param f - 
 * @param vertex_count - Fichier wavefront.
 * @param normal_count - Nombre de vertex.
 * @param tex_coord_count - Nombre de coordonnées de
 * textures.
 * @param face_count - Nombre de faces.
 * @param uv - Les UV.
 * @param normal - Les normales
 * @return int - 1 si la lecture est valide.
 */
static int
model_scan_wavefront(FILE *f, int *vertex_count, int *normal_count,
		     int *tex_coord_count, int *face_count, 
		     int *uv, int *normal)
{
    char line[LINE_SIZE];
    *uv = -1;
    while (fgets(line, LINE_SIZE, f) != NULL) {
	char tmp = line[2];
	line[2] = 0;
	if (strcmp(line, "v ") == 0) {
	    ++(*vertex_count);
	} else if (strcmp(line, "f ") == 0) {
	    line[2] = tmp;
	    if (model_wavefront_face_vertex_count(&line[2]) != 3) {
		fprintf(stderr, "All faces must be triangles.");
		return 0;
	    }
	    ++(*face_count);
	} else if (strcmp(line, "vt") == 0) {
	    *uv = 1;
	    ++(*tex_coord_count);
	} else if (strcmp(line, "vn") == 0) {
	    *normal = 1;
	    ++(*normal_count);
	}
    }
    return 1;
}

/**
 * Charger le wavefront.
 * @param path - Chemin d'accès au fichier.
 * @return struct model * - Le modèle.
 */
struct model *model_load_wavefront(const char *path)
{
    int fd = openat(binary_dir, path, O_RDONLY);
    FILE *f = fdopen(fd, "r");
    if (f == NULL) {
	perror(path);
	exit(EXIT_FAILURE);
    }
    int have_uv = 0, have_normal = 0;
    int vertex_count = 0, normal_count = 0, tex_coord_count = 0;
    int face_count = 0;
    int b = model_scan_wavefront(f, &vertex_count, &normal_count,
				     &tex_coord_count, &face_count,
				     &have_uv, &have_normal);
    if (!b) {
	fprintf(stderr, "%s model load failed.", path);
	return NULL;
    }
    rewind(f);
    vec3 *vertex = malloc(sizeof(*vertex) * vertex_count);
    vec3 *normal = malloc(sizeof(*normal) * normal_count);
    vec2 *tex_coord = malloc(sizeof(*tex_coord) * tex_coord_count);
    
    int *vertex_index = malloc(sizeof(*vertex_index) * face_count * 3);
    int *normal_index = malloc(sizeof(*normal_index) * face_count * 3);
    int *tex_coord_index = malloc(sizeof(*tex_coord_index) * face_count * 3);
    
    normal_count = tex_coord_count = vertex_count = face_count = 0;
    char line[LINE_SIZE];
    while (fgets(line, LINE_SIZE, f) != NULL) {
	char *line_header = strndup(line, 2);
	if (strcmp(line_header, "v ") == 0) {
	    struct vec3 *p = &vertex[vertex_count++];
	    sscanf(&line[2], "%lf %lf %lf\n", &p->x, &p->y, &p->z);
	} else if (strcmp(line_header, "vt") == 0) {
	    struct vec2 *t = &tex_coord[tex_coord_count++];
	    sscanf(&line[2], "%lf %lf\n", &t->x, &t->y);
	    t->y = 1 - t->y;
	    // --> In BLENDER UV coord start up right
	    // OpenGl is Down right
	} else if (strcmp(line_header, "vn") == 0) {
	    struct vec3 *n = &normal[normal_count++];
	    sscanf(&line[2], "%lf %lf %lf\n", &n->x, &n->y, &n->z);
	} else if (strcmp(line_header, "f ") == 0) {
	    int k = 3*face_count++;
	    if (have_uv && have_normal) {
		sscanf(&line[2], "%d/%d/%d %d/%d/%d %d/%d/%d\n",
		       &vertex_index[k], &tex_coord_index[k],
		       &normal_index[k],
		       &vertex_index[k+1], &tex_coord_index[k+1],
		       &normal_index[k+1],
		       &vertex_index[k+2], &tex_coord_index[k+2],
		       &normal_index[k+2]);
	    } else if (have_uv){
		sscanf(&line[2], "%d/%d %d/%d %d/%d\n",
		       &vertex_index[k], &tex_coord_index[k],
		       &vertex_index[k+1], &tex_coord_index[k+1],
		       &vertex_index[k+2], &tex_coord_index[k+2]);
	    } else if (have_normal) {
		sscanf(&line[2], "%d//%d %d//%d %d//%d\n",
		       &vertex_index[k], &normal_index[k],
		       &vertex_index[k+1], &normal_index[k+1],
		       &vertex_index[k+2], &normal_index[k+2]);
	    }
	}
	free(line_header);
    }
    fclose(f);
    struct model *m = malloc(sizeof(*m));
    m->buffer = malloc(sizeof(*m->buffer) * 3*face_count);
    
    for (int i = 0; i < 3*face_count; i++) {
	m->buffer[i].pos = vertex[vertex_index[i]-1];
	if (have_normal)
	    m->buffer[i].normal = normal[normal_index[i]-1];
	if (have_uv)
	    m->buffer[i].tex_coord = tex_coord[tex_coord_index[i]-1];
    }
    m->uv = have_uv; m->normal = have_normal;
    free(vertex); free(vertex_index);
    free(normal); free(normal_index); 
    free(tex_coord); free(tex_coord_index);
    m->vcount = 3*face_count;

    // OpenGL 
    glGenBuffers(1, &m->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m->vbo);
    glBufferData(GL_ARRAY_BUFFER,
		 m->vcount*sizeof(*m->buffer), m->buffer,
		 GL_STATIC_DRAW);
    return m;
}

/**
 * Dessiner le modèle.
 * @param m - Le modèle.
 */
void model_draw(struct model *m)
{
    glBindBuffer(GL_ARRAY_BUFFER, m->vbo);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_DOUBLE, sizeof(struct vertex), NULL);
    if (m->normal) {
	glEnableClientState(GL_NORMAL_ARRAY);
	glNormalPointer(GL_DOUBLE, sizeof(struct vertex),
			(void*)offsetof(struct vertex, normal));
    }
    if (m->uv) {
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_DOUBLE, sizeof(struct vertex),
			  (void*)offsetof(struct vertex, tex_coord));
    }
    glDrawArrays(GL_TRIANGLES, 0, m->vcount);

    if (m->normal)
	glDisableClientState(GL_NORMAL_ARRAY);
    if (m->uv)
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
}

/**
 * Libérer la mémoire occupé par le modèle.
 * @param m - Le modèle.
 */
void model_free(struct model *m)
{
    if (m == NULL)
	return;
    glDeleteBuffers(1, &m->vbo);
    free(m->buffer);
    free(m);
}

/**
 * Afficher le modèle.
 * @param m - Le modèle.
 */
void model_dump(struct model *m)
{
    puts("model_dump START");
    for (int i = 0; i < m->vcount; i++) {
	if (i%3 == 0)
	    printf("triangle %d:\n", i/3);
	struct vertex *v = &m->buffer[i];
	printf("%lf %lf %lf\n"
	       "%lf %lf %lf\n"
	       "%lf %lf\n__\n",
	       v->pos.x, v->pos.y, v->pos.z,
	       v->normal.x, v->normal.y, v->normal.z,
	       v->tex_coord.x, v->tex_coord.y);
	if ((i+1)%3 == 0 && i > 0)
	    puts("------------------");
    }
    puts("model_dump END");
}
