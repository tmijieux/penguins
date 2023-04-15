#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include "d3v/d3v.h"
#include "d3v/camera.h"
#include "utils/math.h"
#include "d3v/d3v_internal.h"
#include "d3v/scene.h"
#include "d3v/shader.h"
#include "d3v/object.h"

#define WINDOW_TITLE "Penguin"
#define WINDOW_POSITION_X 200
#define WINDOW_POSITION_Y 200

#define HEIGHT  700
#define WIDTH   700

GLFWwindow *window;
static int main_loop_quit = 0;
static int main_loop_running = 0;
static int animation_frame_requested = 0;
static int need_redraw = 1;

__internal int _d3v_binary_dir = 0;

void d3v_init_asset_path(int v)
{
    _d3v_binary_dir = v;
}



/* // Helper to check for extension string presence.  Adapted from: */
/* //   http://www.opengl.org/resources/features/OGLextensions/ */
/* static int is_extension_supported(const char *ext_list, const char *extension) */
/* { */
/*     const char *start; */
/*     const char *where, *terminator; */

/*     /\* Extension names should not have spaces. *\/ */
/*     where = strchr(extension, ' '); */
/*     if (where || *extension == '\0') { */
/* 	return 0; */
/*     } */

/*     /\* It takes a bit of care to be fool-proof about parsing the */
/*        OpenGL extensions string. Don't be fooled by sub-strings, */
/*        etc. *\/ */
/*     for (start = ext_list;;) { */
/* 	where = strstr(start, extension); */

/* 	if (!where) { */
/* 	    break; */
/*         } */

/* 	terminator = where + strlen(extension); */

/* 	if (where == start || *(where - 1) == ' ') */
/*         { */
/* 	    if (*terminator == ' ' || *terminator == '\0') { */
/* 		return 1; */
/*             } */
/*         } */
/* 	start = terminator; */
/*     } */
/*     return 0; */
/* } */


static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void window_refresh_callback(GLFWwindow* window)
{
    need_redraw = 1;
}

static void window_pos_callback(GLFWwindow* window, int xpos, int ypos)
{
    need_redraw = 1;
}

static void window_size_callback(GLFWwindow* window, int width, int height)
{
    need_redraw = 1;
}

static void window_close_callback(GLFWwindow *window)
{
    glfwSetWindowShouldClose(window, GLFW_TRUE);
    main_loop_quit = 1;
}

static void window_iconify_callback(GLFWwindow* window, int iconified)
{
    need_redraw = 1;
    /* if (iconified) */
    /* { */
    /*     // The window was iconified */
    /* } */
    /* else */
    /* { */
    /*     // The window was restored */
    /* } */
}


static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    if (action == GLFW_PRESS) {
        d3v_key(scancode, xpos, ypos);
    }

    need_redraw = 1;
}


static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    /* printf("cursor pos = %g %g \n", xpos, ypos); */
    need_redraw = 1;
    d3v_mouse_motion((int) xpos, (int)ypos);
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    need_redraw = 1;

    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    /* printf("mouse button = %d %d %d\n", button,action,mods); */

    d3v_button(button, action, (int)xpos, (int)ypos);
    /* if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) */
    /*     popup_menu(); */
}

static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    /* printf("scroll = x=%.3g y=%.3g \n", xoffset, yoffset); */
    /* d3v_button(, action, 0, 0); */
    if (yoffset > 0) {
        d3v_camera_add_distance(scene.camera, -0.4);
    } else if (yoffset < 0) {
        d3v_camera_add_distance(scene.camera, 0.4);
    }
    need_redraw = 1;
}

static int create_context(void)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow( WIDTH, HEIGHT, WINDOW_TITLE, NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create window\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);

    // callbacks
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // window
    glfwSetWindowSizeCallback(window, window_size_callback);//resized
    glfwSetWindowPosCallback(window, window_pos_callback);//moved
    glfwSetWindowIconifyCallback(window, window_iconify_callback); // minimized/restore
    glfwSetWindowRefreshCallback(window, window_refresh_callback); // "damaged" (hidden/outofsight)
    glfwSetWindowCloseCallback(window, window_close_callback);


    // inputs
    glfwSetKeyCallback(window, key_callback);//keyboard
    glfwSetCursorPosCallback(window, cursor_position_callback);//mousemove
    glfwSetMouseButtonCallback(window, mouse_button_callback);//mouseclick
    glfwSetScrollCallback(window, scroll_callback);//mousescroll

    return 0;
}


/**
 *  Initialisation du module OpenGL.
 */
static void opengl_init(void)
{
    //Initialisation de l'etat d'OpenGL
    HANDLE_GL_ERROR(glClearColor(0.6, 0.9, 0.9, 1.0));
    HANDLE_GL_ERROR(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    /* HANDLE_GL_ERROR(glColor3f(1.0, 1.0, 1.0)); */
    /* HANDLE_GL_ERROR(glShadeModel(GL_SMOOTH)); */
    //glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

    // Parametrage du materiau
    /* HANDLE_GL_ERROR(glEnable(GL_LIGHTING)); */
    /* HANDLE_GL_ERROR(glEnable(GL_COLOR_MATERIAL)); */
    /* HANDLE_GL_ERROR(glEnable(GL_DEPTH_TEST)); */
    /* HANDLE_GL_ERROR(glEnable(GL_NORMALIZE)); */

    /* HANDLE_GL_ERROR(glColorMaterial(GL_FRONT, GL_DIFFUSE)); */
    /* HANDLE_GL_ERROR(glPolygonMode(GL_FRONT, GL_FILL)); */

    /* HANDLE_GL_ERROR(glMatrixMode(GL_MODELVIEW)); */
    /* HANDLE_GL_ERROR(glLoadIdentity()); */
}


void d3v_request_exit_from_main_loop(void)
{
    main_loop_quit = 1;
}

void d3v_request_animation_frame(void)
{
    animation_frame_requested = 1;
    /* need_redraw = 1; */
}

uint32_t VAO, VBO;

void init_data(void)
{
    float vertices[] = {
        // positions          // colors           // texture coords
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
        -0.5f,  0.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
    };


    HANDLE_GL_ERROR(glGenVertexArrays(1, &VAO));
    HANDLE_GL_ERROR(glBindVertexArray(VAO));

    HANDLE_GL_ERROR(glEnableVertexAttribArray(0));
    HANDLE_GL_ERROR(glEnableVertexAttribArray(1));
    HANDLE_GL_ERROR(glEnableVertexAttribArray(2));

    HANDLE_GL_ERROR(glGenBuffers(1, &VBO));
    HANDLE_GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER, VBO));

    HANDLE_GL_ERROR(
        glBufferData(
            GL_ARRAY_BUFFER,
            sizeof(vertices),
            vertices,
            GL_STATIC_DRAW
        )
    );
    HANDLE_GL_ERROR(
        glVertexAttribPointer(
            0, 3, GL_FLOAT, GL_FALSE,
            8 * sizeof(float),
            (void*)0
        )
    );
    HANDLE_GL_ERROR(
        glVertexAttribPointer(
            1, 3, GL_FLOAT, GL_FALSE,
            8 * sizeof(float),
            (void*)(3 * sizeof(float))
        )
    );
    HANDLE_GL_ERROR(
        glVertexAttribPointer(
            2, 2, GL_FLOAT, GL_FALSE,
            8 * sizeof(float),
            (void*)(6 * sizeof(float))
        )
    );

    /* HANDLE_GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER, 0)); */
    HANDLE_GL_ERROR(glBindVertexArray(0));
}



static void GLAPIENTRY
MessageCallback( GLenum source,
                 GLenum type,
                 GLuint id,
                 GLenum severity,
                 GLsizei length,
                 const GLchar* message,
                 const void* userParam )
{
    fprintf(
        stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
        (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
        type, severity, message
    );
}

static void opengl_init_error(void)
{
    HANDLE_GL_ERROR(glEnable(GL_DEBUG_OUTPUT));
    HANDLE_GL_ERROR(glDebugMessageCallback( MessageCallback, 0));
}

static void opengl_displayinfo(void)
{
    printf("GL_VERSION  = %s\n", glGetString(GL_VERSION));
    printf("GL_RENDERER = %s\n", glGetString(GL_RENDERER));
    printf("GL_VENDOR   = %s\n", glGetString(GL_VENDOR));

    int nrAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
    printf("Maximum nr of vertex attributes supported: %d\n", nrAttributes);
}

// PUBLIC
int d3v_module_init(int object_count_clue)
{
    create_context();
    opengl_displayinfo();
    opengl_init_error();
    opengl_init();
    d3v_scene_module_init(object_count_clue);
    return 0;
}

int d3v_set_initial_camera_position(const vec3 *pos)
{
    d3v_camera_set_look(scene.camera, pos);
    scene.first_look = *pos;
}


/**
 * Dessine les bases : (O, x,y,z)
 */
static void create_world_basis(void)
{
    float basis_data[] = {
        // coord          // color
        0.0, 0.0, 0.0,    1.0, 0.0, 0.0, // red   x
        1.0, 0.0, 0.0,    1.0, 0.0, 0.0, // red   x
        0.0, 0.0, 0.0,    0.0, 1.0, 0.0, // green y
        0.0, 1.0, 0.0,    0.0, 1.0, 0.0, // green y
        0.0, 0.0, 0.0,    0.0, 0.0, 1.0, // blue  z
        0.0, 0.0, 1.0,    0.0, 0.0, 1.0, // blue  z
    };
    model_t *model = create_model_from_data(
        basis_data, 6, sizeof(basis_data), GL_LINES,
        0, 1, 0,
        0, 3*sizeof(float), 0,
        6*sizeof(float)
    );

    vec3 p = {0,0,0};
    int program = get_or_load_shader(PENGUIN_SHADER_COLOR_NO_MODEL);
    object_t *obj = d3v_object_create(model, NULL, p, p, p, program, "world basis");
    d3v_add_object(obj);

    /* program = get_or_load_shader(PENGUIN_SHADER_COLOR_NO_PROJ); */
    /* obj = d3v_object_create(model, NULL, p, p, p, program, "clip basis"); */
    /* d3v_add_object(obj); */
}


int d3v_main_loop(void)
{
    main_loop_running = 1;
    glfwMakeContextCurrent(window);

    /* init_data(); */
    HANDLE_GL_ERROR(glEnable(GL_DEPTH_TEST));
    create_world_basis();

    while (!main_loop_quit) {

        glfwPollEvents();

        if (need_redraw) {
            need_redraw = 0;

            HANDLE_GL_ERROR(glClearColor(0.6, 0.9, 0.9, 1.0));
            HANDLE_GL_ERROR(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

            d3v_scene_draw();

            /* mat4 model; */
            /* uint32_t k2 = glGetUniformLocation(3, "model"); */
            /* make_identity(&model); */
            /* glUniformMatrix4fv(k2, 1, GL_TRUE, model.m); */
            /* HANDLE_GL_ERROR(glBindVertexArray(VAO)); */
            /* HANDLE_GL_ERROR(glDrawArrays(GL_TRIANGLES, 0, 3)); */

            glfwSwapBuffers(window);

        }
        if (animation_frame_requested) {
            animation_frame_requested = 0;
            need_redraw = 1;
        }
        sleep(1.0 / 60.0);
    }

    d3v_scene_module_exit();
    glfwDestroyWindow(window);
    glfwTerminate();

    main_loop_running = 0;
    return 0;
}
