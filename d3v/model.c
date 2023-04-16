/**
 * @file model.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <stddef.h>

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>

#include "utils/vec.h"
#include "d3v/model.h"
#include "d3v/d3v.h"
#include "d3v/d3v_internal.h"

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
    uint32_t nb_vertices;
    GLenum primitive;
    GLuint vao, vbo;
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
    while (fgets(line, LINE_SIZE, f) != NULL)
    {
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
    int fd = openat(_d3v_binary_dir, path, O_RDONLY);
    if (fd == -1) {
        perror(path);
        exit(EXIT_FAILURE);
    }

    FILE *f = fdopen(fd, "r");
    if (f == NULL) {
        perror(path);
        exit(EXIT_FAILURE);
    }
    int have_uv = 0, have_normal = 0;
    int nb_vertices = 0, nb_normal = 0, nb_texcoord = 0;
    int face_count = 0;
    int b = model_scan_wavefront(f, &nb_vertices, &nb_normal,
                                 &nb_texcoord, &face_count,
                                 &have_uv, &have_normal);
    if (!b) {
	fprintf(stderr, "%s model load failed.", path);
	return NULL;
    }
    rewind(f);
    vec3 *vertex = malloc(sizeof(*vertex) * nb_vertices);
    vec3 *normal = malloc(sizeof(*normal) * nb_normal);
    vec2 *tex_coord = malloc(sizeof(*tex_coord) * nb_texcoord);

    int *vertex_index = malloc(sizeof(*vertex_index) * face_count * 3);
    int *normal_index = malloc(sizeof(*normal_index) * face_count * 3);
    int *tex_coord_index = malloc(sizeof(*tex_coord_index) * face_count * 3);

    nb_normal = nb_texcoord = nb_vertices = face_count = 0;
    char line[LINE_SIZE];
    while (fgets(line, LINE_SIZE, f) != NULL) {
	char *line_header = strndup(line, 2);
	if (strcmp(line_header, "v ") == 0) {
	    struct vec3 *p = &vertex[nb_vertices++];
	    sscanf(&line[2], "%f %f %f\n", &p->x, &p->y, &p->z);
	} else if (strcmp(line_header, "vt") == 0) {
	    struct vec2 *t = &tex_coord[nb_texcoord++];
	    sscanf(&line[2], "%f %f\n", &t->x, &t->y);
	    t->y = 1 - t->y;
	    // --> In BLENDER UV coord start up right
	    // OpenGl is Down right
	} else if (strcmp(line_header, "vn") == 0) {
	    struct vec3 *n = &normal[nb_normal++];
	    sscanf(&line[2], "%f %f %f\n", &n->x, &n->y, &n->z);
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

    nb_vertices = face_count * 3;
    size_t buffer_size = sizeof(struct vertex) * nb_vertices;
    struct vertex *buffer = malloc(buffer_size);
    for (int i = 0; i < nb_vertices; i++)
    {
	buffer[i].pos = vertex[vertex_index[i]-1];
	if (have_normal) {
	    buffer[i].normal = normal[normal_index[i]-1];
        }
	if (have_uv) {
	    buffer[i].tex_coord = tex_coord[tex_coord_index[i]-1];
        }
    }

    free(vertex);
    free(vertex_index);
    free(normal);
    free(normal_index);
    free(tex_coord);
    free(tex_coord_index);

    // OpenGL
    model_t *model = create_model_from_data(
        buffer, nb_vertices, buffer_size, GL_TRIANGLES,
        have_uv, 0, have_normal,
        offsetof(struct vertex, tex_coord), 0, offsetof(struct vertex, normal),
        sizeof(struct vertex)
    );
    free(buffer);
    return model;
}


/**
 * Charger le wavefront.
 * @param path - Chemin d'accès au fichier.
 * @return struct model * - Le modèle.
 */
model_t *create_model_from_data(
    void *data, uint32_t nb_vertices, size_t buffer_size, int gl_primitive,
    int have_uv, int have_color, int have_normals,
    size_t uv_offset, size_t color_offset, size_t normals_offset, int stride)
{
    model_t *model = calloc(1, sizeof(*model));
    model->primitive = gl_primitive;
    model->nb_vertices = nb_vertices;

    model->vao = 0;
    HANDLE_GL_ERROR(glGenVertexArrays(1, &model->vao));
    HANDLE_GL_ERROR(glBindVertexArray(model->vao));


    HANDLE_GL_ERROR(glEnableVertexAttribArray(0)); // vertex data
    if (have_normals) {
        HANDLE_GL_ERROR(glEnableVertexAttribArray(1));
    }
    if (have_uv) {
        HANDLE_GL_ERROR(glEnableVertexAttribArray(2));
    }
    if (have_color) {
        HANDLE_GL_ERROR(glEnableVertexAttribArray(3));
    }

    HANDLE_GL_ERROR(glGenBuffers(1, &model->vbo));
    HANDLE_GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER, model->vbo));

    HANDLE_GL_ERROR(glBufferData(GL_ARRAY_BUFFER, buffer_size, data, GL_STATIC_DRAW));
    HANDLE_GL_ERROR(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,  stride, (void*)0));

    if (have_normals) {
        HANDLE_GL_ERROR(
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)normals_offset)
        );
    }
    if (have_uv) {
        HANDLE_GL_ERROR(
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)uv_offset)
        );
    }
    if (have_color) {
        HANDLE_GL_ERROR(
            glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (void*) color_offset)
        );
    }
    /* HANDLE_GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER, 0)); */
    HANDLE_GL_ERROR(glBindVertexArray(0));
    return model;
}


/**
 * Dessiner le modèle.
 * @param m - Le modèle.
 */
void model_draw(struct model *m)
{
    HANDLE_GL_ERROR(glBindVertexArray(m->vao));
    HANDLE_GL_ERROR(glDrawArrays(m->primitive, 0, m->nb_vertices));
}

/**
 * Libérer la mémoire occupé par le modèle.
 * @param m - Le modèle.
 */
void model_free(struct model *model)
{
    if (model == NULL) {
	return;
    }
    glDeleteBuffers(1, &model->vbo);
    glDeleteVertexArrays(1, &model->vao);
    memset(model, 0, sizeof *model);
    free(model);
}

/**
 * Afficher le modèle.
 * @param m - Le modèle.
 */
void model_dump(struct model *m)
{
    puts("model");
    printf("nb_vertices=%d\n", m->nb_vertices);
    printf("mode=%d\n", m->primitive);
    printf("vbo=%d\n", m->vbo);
    printf("vao=%d\n", m->vao);
}
