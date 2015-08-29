#include <pthread.h>
#include <math.h>

#include <utils/vec.h>
#include <utils/math.h>

#include <d3v/mouse_projection.h>

#include <display/dsp.h>
#include <display/dtile.h>
#include <display/mouseclick.h>

#define SHORT_DISTANCE 0.4

static struct {
    pthread_mutex_t mut;
    pthread_cond_t cond;
    pthread_attr_t attr;
    pthread_condattr_t condattr;
    pthread_mutexattr_t mutattr;
    vec3 pos;
} mp;


void dsp_signal_game_thread(vec3 *pos)
{
    // display thread
    mp.pos = *pos;
    pthread_cond_signal(&mp.cond);
}

static int get_tile_by_pos(const vec3 *pos)
{
    int tile = -1;
    float min_dis = 999999.f;
    for (int i = 0; i < dsp.tile_count; ++i) {
	vec3 p; dtile_get_position(dsp.tiles[i], &p);
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

void display_mc_init(int (*coord_on_tile)(double x, double z),
		     double z_shoes, double z_feet, double z_head)
{
    // game thread
    
}

static int dsp_run = 0;

void dsp_set_thread_running_state(int running)
{
    dsp_run = running;
}

// PUBLIC (game thread)
int display_mc_get(struct mouseclick *mc)
{
    // game thread
    if (!mc)
	return INVALID_MOUSECLICK_STRUCT;
    if (!dsp.thread_running)
    	return DISPLAY_THREAD_STOP;
    
    dsp.mouseclick_mode = 1;
    
    pthread_mutexattr_init(&mp.mutattr);
    pthread_mutex_init(&mp.mut, &mp.mutattr);
    
    pthread_condattr_init(&mp.condattr);
    pthread_cond_init(&mp.cond, &mp.condattr);
    
    pthread_mutex_lock(&mp.mut);
    pthread_cond_wait(&mp.cond, &mp.mut);
    dsp.mouseclick_mode = 0;
    
    pthread_cond_destroy(&mp.cond);
    
    if (!dsp.thread_running)
	return DISPLAY_THREAD_STOP;

    if (mp.pos.x == -INFINITY)
	return SURRENDER;
    
    if (!(mc->validclick = check_boundaries(&mp.pos)))
	return INVALID_CLICK;
    
    int tile = get_tile_by_pos(&mp.pos);
    mc->tile_id = tile;
    if ((mc->peng_id = dtile_get_penguin(dsp.tiles[tile])) != -1)
	mc->t = MC_PENGUIN;
    else
	mc->t = MC_TILE;
    return 0;
}
