#ifndef DSP_H
#define DSP_H


#include <utils/vec.h>
#include <utils/list.h>

struct display {
    int thread_running;
    
//Plateau
    struct dtile **tiles;
    int tile_count;

    struct model *penguin_model;
    struct texture **penguin_tex;
    int penguin_tex_count;

    struct dpenguin **penguins;
    int nb_peng_alloc;
    int penguin_count;
    vec3 centroid;

// move's list/queue
    struct record *rec;
    
// Affichage Direction
    int linked;
    int **link;
    int activeLink;

    int autoplay;
    int mouseclick_mode;

    struct list *tex_list;
    struct list *mod_list;
    
};

extern struct display dsp;

void dsp_signal_game_thread(vec3 *pos);
void dsp_set_thread_running_state(int running);

#endif //DSP_H
