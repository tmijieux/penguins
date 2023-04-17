#ifndef DISPLAY_DSP_H
#define DISPLAY_DSP_H

#include <stdint.h>

#include "utils/vec.h"
#include "utils/list.h"

struct record;
struct object;

#define DISPLAY_TEXTURE_MAX 30
#define DISPLAY_MODEL_MAX 30

struct display {
    int thread_running;
    int thread_terminated;
    int nb_dimension;

    // Plateau

    int nb_tile;
    int nb_tile_alloc;
    struct object **tile_objects;
    int *tile_nb_fishes;
    int *tile_penguins_id;
    /* int *tile_models; */
    /* int *tile_textures; */

    vec3 centroid;

    struct model *penguin_model;
    struct texture **penguin_textures;
    int nb_penguin_tex;

    int nb_penguin;
    int nb_penguin_alloc;
    struct object **penguin_objects;


    // move's list/queue
    struct record *records;

    // Affichage Direction
    int **link; // link[i][j] = 1 if edge (i,j) in graph
    int should_draw_links;
    int active_tile_id;
    int nb_links;
    uint32_t link_vbo, link_vao;
    uint32_t link_program;

    int autoplay;
    int mouseclick_mode;


    int nb_registered_textures;
    int nb_registered_models;
    const char *textures[DISPLAY_TEXTURE_MAX];
    const char *models[DISPLAY_MODEL_MAX];

    struct model **tile_models;
    struct texture **tile_textures;
};

extern struct display Display;

void dsp_signal_game_thread(vec3 *pos);
void dsp_set_thread_running_state(int running);

#endif // DISPLAY_DSP_H
