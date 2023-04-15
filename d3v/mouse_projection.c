#include <stdio.h>
#include <math.h>
#include <pthread.h>

#include <GL/glu.h>

#include <utils/math.h>
#include <utils/vec.h>

#include <d3v/scene.h>
#include <d3v/mouse_projection.h>

// NOT PRIVATE, may be used in the mouse event callback !!
// NEVER in any other place !!

int d3v_mouseproj(vec3 *pos, int x, int y)
{
    // display thread
    GLdouble ox = 0.0, oy = 0.0, oz = 0.0;
    GLint viewport[4];
    /* GLdouble view[16], projection[16]; */
    GLfloat wx, wy, wz;


    mat4d view, proj;
    camera_t *camera = scene.camera;
    d3v_camera_get_view_matrix_d(camera, &view);
    d3v_camera_get_proj_matrix_d(camera, &proj);

    glGetIntegerv(GL_VIEWPORT, viewport);
    wx = x;
    wy = y = viewport[3] - y;

    glReadPixels(x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &wz);
    gluUnProject(wx, wy, wz,
		 view.m, proj.m, viewport,
		 &ox, &oy, &oz);

    *pos = (vec3) { ox, oy, oz };
    return 1;
}
