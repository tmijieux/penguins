#include <stdio.h>
#include <math.h>

#include <GL/freeglut.h>
#include <GL/glu.h>

#include <utils/vec.h>

int mouse_projection(vec3 *pos, int x, int y)
{
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

/* // square only */
/* int display_get_targeted_tile(vec3 *target_pos) */
/* { */
/*     for (int i = 0; i < scene.tile_count; i++) { */
/* 	vec3 pos; */
/* 	dtile_get_pos(scene.tiles[i], &pos); */
/* 	if (dnorm1(&pos, target_pos) < 1) */
/* 	    return i; */
/*     } */
/*     return -1; */
/* } */

