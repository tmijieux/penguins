#include "penguins_opengl.h"

#include "utils/vec.h"

#include "d3v/camera.h"
#include "d3v/scene.h"
#include "d3v/mouse_projection.h"

// NOT PRIVATE, may be used in the mouse event callback !!
// NEVER in any other place !!

int d3v_mouseproj(vec3 *pos, int x, int y)
{
    mat4d view, proj;
    camera_t *camera = scene.camera;
    d3v_camera_get_view_matrix_d(camera, &view);
    d3v_camera_get_proj_matrix_d(camera, &proj);

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    GLfloat wx, wy, wz;
    wx = (float)x;
    wy = (float)(viewport[3] - y);
    glReadPixels(wx, wy, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &wz);

    // display thread
    GLdouble ox = 0.0, oy = 0.0, oz = 0.0;
    gluUnProject(wx, wy, wz,
		 view.m, proj.m, viewport,
		 &ox, &oy, &oz);

    *pos = (vec3) { ox, oy, oz };
    return 1;
}
