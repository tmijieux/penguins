#include <math.h>

#include "utils/threading.h"
#include "utils/vec.h"
#include "utils/math.h"

#include "d3v/mouse_projection.h"
#include "d3v/object.h"

#include "display/display_internal.h"
#include "display/mouseclick.h"


struct _mouseclick {
    peng_mutex_t mut;
    peng_condition_t cond;
    vec3 pos;
};
static struct _mouseclick mp;

static int dsp_run = 0;

static int get_tile_by_pos(const vec3 *pos)
{
    int tile = -1;
    float min_dis = 999999.f;
    for (int i = 0; i < Display.nb_tile_alloc; ++i) {

        vec3 p;
        d3v_object_get_position(Display.tile_objects[i], &p);

        float dis;
        if ((dis = math_euclidian_distance(pos, &p)) < min_dis){
            tile = i;
            min_dis = dis;
        }
    }
    return tile;
}

static int check_boundaries(vec3 *pos)
{
    // TODO: implements
    return 1; // return 1 for test
    // same as if user never click on the background
}


__internal
void dsp_set_thread_running_state(int running)
{
    dsp_run = running;
}

__internal
void dsp_signal_game_thread(vec3 *pos)
{
    // display thread
    mp.pos = *pos;
    peng_cond_signal(&mp.cond);
}

// called from game thread (by client modules)
int display_mc_get(struct mouseclick *out)
{
    if (!out) {
        return TC_INVALID_MOUSECLICK_STRUCT;
    }
    if (!Display.thread_running) {
        return TC_DISPLAY_THREAD_STOP;
    }

    Display.mouseclick_mode = 1;

    peng_mutex_init(&mp.mut);
    peng_cond_init(&mp.cond);

    peng_mutex_lock(&mp.mut);
    peng_cond_wait(&mp.cond, &mp.mut);

    Display.mouseclick_mode = 0;

    peng_mutex_unlock(&mp.mut);
    peng_cond_destroy(&mp.cond);
    peng_mutex_destroy(&mp.mut);

    if (!Display.thread_running) {
        return TC_DISPLAY_THREAD_STOP;
    }

    vec3 pos = mp.pos;

    if (pos.x == -INFINITY) {
        return TC_SURRENDER;
    }

    if (!(out->valid_click = check_boundaries(&pos))) {
        return TC_INVALID_CLICK;
    }

    int tile = get_tile_by_pos(&pos);
    out->tile_id = tile;

    if ((out->peng_id = Display.tile_penguins_id[tile]) != -1) {
        out->object_type = MC_PENGUIN;
    } else {
        out->object_type = MC_TILE;
    }
    Display.autoplay = 1;
    return 0;
}
