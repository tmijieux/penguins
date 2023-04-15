#include <pthread.h>
#include <math.h>

#include "utils/vec.h"
#include "utils/math.h"

#include "d3v/mouse_projection.h"
#include "d3v/object.h"

#include "display/display_internal.h"
#include "display/mouseclick.h"

struct _mouseclick {
    pthread_mutex_t mut;
    pthread_cond_t cond;
    pthread_attr_t attr;
    pthread_condattr_t condattr;
    pthread_mutexattr_t mutattr;
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

static void mutex_init(void)
{
    pthread_mutexattr_init(&mp.mutattr);
    pthread_mutex_init(&mp.mut, &mp.mutattr);

    pthread_condattr_init(&mp.condattr);
    pthread_cond_init(&mp.cond, &mp.condattr);
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
    pthread_cond_signal(&mp.cond);
}

void display_mc_init(int (*coord_on_tile)(double x, double z),
		     double z_shoes, double z_feet, double z_head)
{

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
    mutex_init();
    pthread_mutex_lock(&mp.mut);

    pthread_cond_wait(&mp.cond, &mp.mut);
    Display.mouseclick_mode = 0;
    pthread_cond_destroy(&mp.cond);

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
    return 0;
}