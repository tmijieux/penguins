/**
 * @file texture.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <jpeglib.h>
#include <fcntl.h>

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>

#include <d3v/texture.h>
#include <d3v/d3v.h>

/**
 * Description d'un texture.
 */
struct texture {
    GLuint tex_id;
    unsigned char *tex_data;
};

/**
 * Chargement d'une image au format JPG.
 * @param name - Nom du fichier à charger.
 * @param width_ - Largeur du fichier chargé.
 * @param height_ - Hauteur du fichier chargé.
 * @return static unsigned char * - Données de la texture.
 */
static unsigned char
*texture_load_jpeg(const char *name, int *width_, int *height_)
{
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    FILE*f;
    unsigned char *line;

    int fd = openat(_d3v_binary_dir, name, O_RDONLY);
    if ((f = fdopen(fd, "rb")) == NULL) {
	perror(name);
	exit(EXIT_FAILURE);
    }
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, f);
    jpeg_read_header(&cinfo, TRUE);

    if (cinfo.jpeg_color_space == JCS_GRAYSCALE) {
	fprintf(stdout, "Erreur: %s doit etre de type RGB\n", name);
	exit(EXIT_FAILURE);
    }

    int width = cinfo.image_width;
    int height = cinfo.image_height;

    unsigned char *texture = malloc(height * width * 3);

    jpeg_start_decompress(&cinfo);
    line = texture;
    while (cinfo.output_scanline < cinfo.output_height) {
	line = texture + 3 * width * cinfo.output_scanline;
	jpeg_read_scanlines(&cinfo, &line, 1);
    }
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    fclose(f);

    *width_ = width; *height_ = height;
    return texture;
}

/**
 * Chargement d'un texture.
 * @param path - Chemin d'accès du fichier à charger.
 * @return struct texture * - Texture chargé.
 */
struct texture *texture_load(const char *path)
{
    struct texture *t = malloc(sizeof(*t));
    glGenTextures(1, &t->tex_id);
    glBindTexture(GL_TEXTURE_2D, t->tex_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    int width, height;
    t->tex_data = texture_load_jpeg(path, &width, &height);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height,
		 0, GL_RGB, GL_UNSIGNED_BYTE, t->tex_data);
    return t;
}

/**
 * Obtenir l'identifiant d'une texture.
 * @param t - La texture.
 * @return GLuint - Identifiant de la texture.
 */
GLuint texture_get_id(struct texture *t)
{
    return t->tex_id;
}

/**
 * Libérer la mémoire occupé par la texture.
 * @param t - La texture.
 */
void texture_free(struct texture *t)
{
    if (t == NULL)
	return;
    glDeleteTextures(1, &t->tex_id);
    free(t->tex_data);
    free(t);
}
