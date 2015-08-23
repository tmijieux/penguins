#include <stdio.h>
#include <math.h>
#include <pthread.h>

#include <GL/freeglut.h>
#include <GL/glu.h>

#include <utils/math.h>
#include <utils/vec.h>

#include "display.h"
#include "mouse_projection.h"

#define SHORT_DISTANCE   0.4

static struct {
    pthread_mutex_t mut;
    pthread_cond_t cond;
    pthread_attr_t attr;
    pthread_condattr_t condattr;
    pthread_mutexattr_t mutattr;
    vec3 pos;
} mp;

static int get_tile_by_pos(const vec3 *pos)
{
    for (int i = 0; i < scene.tile_count; ++i) {
	vec3 p; dtile_get_position(scene.tiles[i], &p);
	if (math_euclidian_distance(pos, &p) < SHORT_DISTANCE)
	    return i;
    }
    return -1;
}

static int check_boundaries()
{
    return 0;
}

// PUBLIC (game thread)
int display_mc_get(struct mouseclick *mc)
{
    // game thread
    if (!mc)
	return -2;
    
    pthread_cond_init(&mp.cond, &mp.condattr);
    scene_mouseclick_mode(1);
	
    pthread_mutex_lock(&mp.mut);
    pthread_cond_wait(&mp.cond, &mp.mut);
    scene_mouseclick_mode(0);
    pthread_cond_destroy(&mp.cond);
    
    if (!(mc->validclick = check_boundaries(&mp.pos)))
	return -1;
    
    int tile = get_tile_by_pos(&mp.pos);
    mc->tile_id = tile;
    if ((mc->peng_id = dtile_get_penguin(scene.tiles[tile])) != -1)
	mc->t = MC_PENGUIN;
    else
	mc->t = MC_TILE;
    return 0;
}

void display_mc_init(int (*coord_on_tile)(double x, double z),
		     double z_shoes, double z_feet, double z_head)
{
    // game thread
    pthread_attr_init(&mp.attr);
    pthread_condattr_init(&mp.condattr);
    pthread_mutexattr_init(&mp.mutattr);
    pthread_mutex_init(&mp.mut, &mp.mutattr);
    
}

// PRIVATE
int mc_mouse_projection(vec3 *pos, int x, int y)
{
    // display thread
    GLdouble ox = 0.0, oy = 0.0, oz = 0.0;
    GLint viewport[4];
    GLdouble modelview[16], projection[16];
    GLfloat wx = x, wy, wz;
    glGetIntegerv(GL_VIEWPORT, viewport);
    wy = y =  viewport[3] - y;
    
    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
	    
    glReadPixels(x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &wz);
    gluUnProject(wx, wy, wz,
		 modelview, projection, viewport,
		 &ox, &oy, &oz);
    *pos = (vec3) { ox, oy, oz };
    return 1;
}

void mc_cond_signal(void)
{
    // display thread
    pthread_cond_signal(&mp.cond);
}

void mc_set_pos(const vec3 *pos)
{
    mp.pos = *pos;
}
