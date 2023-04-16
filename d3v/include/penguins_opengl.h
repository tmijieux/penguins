#ifndef PENGUINS_OPENGL_H
#define PENGUINS_OPENGL_H

#ifdef _WIN32
#include <Windows.h>
#endif

#ifndef _WIN32
# define GL_GLEXT_PROTOTYPES
# include <GLFW/glfw3.h>
# include <GL/gl.h>
# include <GL/glu.h>
#else
# include <glad/glad.h>
# define GLFW_INCLUDE_NONE
# include <GLFW/glfw3.h>
#include <GL/glu.h>

/* # include <GL/gl.h> */
/* # include <GL/glu.h> */
/* # include <GL/glext.h> */
/* # include <GL/glcorearb.h> */
#endif

#endif // PENGUINS_OPENGL_H
