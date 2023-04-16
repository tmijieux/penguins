/**
 * @file texture.c
 */
#ifdef _WIN32
#include <Windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
// #include <jpeglib.h> // FIXME
#include <fcntl.h>

#include "penguins_opengl.h"

#include "stb_image.h"

#include "d3v/d3v_internal.h"
#include "d3v/texture.h"
#include "d3v/d3v.h"

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
static
uint8_t*
texture_load_jpeg(const char *path, int *width_, int *height_)
{
    unsigned char *data;
    FILE *f;
    if ((f = fopen(path, "rb")) == NULL) {
	perror(path);
	exit(EXIT_FAILURE);
    }
    int nrChannels;
    data = stbi_load_from_file(f, width_, height_, &nrChannels, 0);
    fclose(f);

    return data;
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
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGB,
        width, height,
        0, GL_RGB, GL_UNSIGNED_BYTE,
        t->tex_data
    );
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
