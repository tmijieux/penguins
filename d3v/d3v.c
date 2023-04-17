#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "penguins_opengl.h"

#ifdef _WIN32
# define sleep(x) Sleep( (int)((x) * 1000))
#else
# include <unistd.h> // for sleep
#endif

#include <ft2build.h>
#include FT_FREETYPE_H

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


/* // Helper to check for extension string presence.  Adapted from: */
/* //   http://www.opengl.org/resources/features/OGLextensions/ */
/* static int is_extension_supported(const char *ext_list, const char *extension) */
/* { */
/*     const char *start; */
/*     const char *where, *terminator; */

/*     /\* Extension names should not have spaces. *\/ */
/*     where = strchr(extension, ' '); */
/*     if (where || *extension == '\0') { */
/*      return 0; */
/*     } */

/*     /\* It takes a bit of care to be fool-proof about parsing the */
/*        OpenGL extensions string. Don't be fooled by sub-strings, */
/*        etc. *\/ */
/*     for (start = ext_list;;) { */
/*      where = strstr(start, extension); */

/*      if (!where) { */
/*          break; */
/*         } */

/*      terminator = where + strlen(extension); */

/*      if (where == start || *(where - 1) == ' ') */
/*         { */
/*          if (*terminator == ' ' || *terminator == '\0') { */
/*              return 1; */
/*             } */
/*         } */
/*      start = terminator; */
/*     } */
/*     return 0; */
/* } */


static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    //glViewport(0, 0, width, height);
    d3v_reshape(width, height);
    // call d3v_reshape
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

    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        d3v_key(key, scancode, action, mods, xpos, ypos);
    }

    need_redraw = 1;
}


static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    need_redraw = 1;
    d3v_mouse_motion((int) xpos, (int)ypos);
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    need_redraw = 1;

    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    d3v_button(button, action, mods, (int)xpos, (int)ypos);
    /* if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) */
    /*     popup_menu(); */
}

static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    /* d3v_button(, action, 0, 0); */
    if (yoffset > 0) {
        d3v_camera_add_distance(scene.camera, -0.4);
    } else if (yoffset < 0) {
        d3v_camera_add_distance(scene.camera, +0.4);
    }
    need_redraw = 1;
}

static int create_context(void)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(WIDTH, HEIGHT, WINDOW_TITLE, NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create window\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwSetWindowPos(window, WINDOW_POSITION_X, WINDOW_POSITION_Y);
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        fprintf(stderr, "Failed to initialize GLAD\n");
        exit(EXIT_FAILURE);
    }

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

void d3v_request_exit_from_main_loop(void)
{
    main_loop_quit = 1;
}

void d3v_request_animation_frame(void)
{
    animation_frame_requested = 1;
    /* need_redraw = 1; */
}

#ifdef PENGUINS_HAS_OPENGL_4_3
static void GLAPIENTRY
MessageCallback( GLenum source,
                 GLenum type,
                 GLuint id,
                 GLenum severity,
                 GLsizei length,
                 const GLchar* message,
                 const void* userParam )
{
    //fprintf(
    //    stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
    //    (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
    //    type, severity, message
    //);
    fprintf(
        stderr, "GL CALLBACK: type = 0x%x, severity = 0x%x, message = %s\n",
        type, severity, message
    );
}
#endif  // PENGUINS_HAS_OPENGL_4_3

static void opengl_init_error(void)
{
    #ifdef PENGUINS_HAS_OPENGL_4_3
    /* DEBUG only in OpenGL 4.3 */
    HANDLE_GL_ERROR(glEnable(GL_DEBUG_OUTPUT));
    HANDLE_GL_ERROR(glDebugMessageCallback( MessageCallback, 0));
    #endif  // PENGUINS_HAS_OPENGL_4_3
}

static void opengl_displayinfo(void)
{
#ifdef DEBUG
    printf("GL_VERSION  = %s\n", glGetString(GL_VERSION));
    printf("GL_RENDERER = %s\n", glGetString(GL_RENDERER));
    printf("GL_VENDOR   = %s\n", glGetString(GL_VENDOR));

    int nrAttributes = 0;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
    printf("Maximum nr of vertex attributes supported: %d\n", nrAttributes);
#endif
}

// PUBLIC
int d3v_module_init(int object_count_clue)
{
    create_context();
    opengl_displayinfo();
    opengl_init_error();
    d3v_scene_module_init(object_count_clue);
    return 0;
}

void d3v_set_initial_camera_position(const vec3 *pos)
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

typedef struct Character {
    uint32_t TextureID;  // ID handle of the glyph texture
    ivec2   Size;       // Size of glyph
    ivec2   Bearing;    // Offset from baseline to left/top of glyph
    uint32_t Advance;    // Offset to advance to next glyph
} Character;

Character characters[20];

void init_freetype(void)
{
    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        fprintf(stderr, "Could not init freetype!!\n");
        exit(EXIT_FAILURE);
    }
    FT_Face face;
    const char *fontpath = "fonts/OpenSans-Regular.ttf";
    if (FT_New_Face(ft, fontpath, 0, &face)) {
        fprintf(stderr, "Could not load font %s!!\n", fontpath);
        exit(EXIT_FAILURE);
    }
    FT_Set_Pixel_Sizes(face, 0, 48);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction
    for (char c = '0'; c <= '9'; ++c) {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            fprintf(stderr, "Failed to load glyph %c!!\n", c);
            exit(EXIT_FAILURE);
        }
        uint32_t texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );
        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // now store character for later use
        Character character = {
            texture,
            (ivec2){face->glyph->bitmap.width, face->glyph->bitmap.rows},
            (ivec2){face->glyph->bitmap_left, face->glyph->bitmap_top},
            face->glyph->advance.x
        };
        characters[c-'0'] = character;
    }
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}


uint32_t text_VAO, text_VBO;
uint32_t text_SHADER;

void prepare_text_rendering(void)
{
    text_SHADER = get_or_load_shader(PENGUIN_SHADER_TEXT);
    glGenVertexArrays(1, &text_VAO);
    glGenBuffers(1, &text_VBO);

    glBindVertexArray(text_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, text_VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void d3v_draw_text(char c, vec3 pos)
{
    const float scale = 1.0;
    if (c < '1' || c > '9') {
        fprintf(stderr, "Cannot render other than digits between '1', and '9'\n");
        return;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(text_SHADER);
    //glUniform3f(glGetUniformLocation(text_SHADER, "textColor"), color.x, color.y, color.z);

    mat4 trans;
    make_translation_matrix(pos, &trans);
    glUniformMatrix4fv(glGetUniformLocation(text_SHADER, "model"), 1, GL_TRUE, trans.m);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(text_VAO);

    // iterate through all characters
    Character ch = characters[c-'0'];

    float w = ch.Size.x * scale;
    float h = ch.Size.y * scale;
    w /= 200;
    h /= 200;

    float x = -w/2;
    float y = -h/2;

    float xpos = x + (ch.Bearing.x * scale) / 200;
    float ypos = y - ((ch.Size.y - ch.Bearing.y) * scale) / 200;

    // update VBO for each character
    float vertices[6][4] = {
        { xpos,     ypos + h,   0.0f, 0.0f },
        { xpos,     ypos,       0.0f, 1.0f },
        { xpos + w, ypos,       1.0f, 1.0f },

        { xpos,     ypos + h,   0.0f, 0.0f },
        { xpos + w, ypos,       1.0f, 1.0f },
        { xpos + w, ypos + h,   1.0f, 0.0f }
    };
    // render glyph texture over quad
    glBindTexture(GL_TEXTURE_2D, ch.TextureID);
    // update content of VBO memory
    glBindBuffer(GL_ARRAY_BUFFER, text_VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // render quad
    glDrawArrays(GL_TRIANGLES, 0, 6);
    // now advance cursors for next glyph (note that advance is number of 1/64 pixels)

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

int d3v_main_loop(void)
{
    main_loop_running = 1;
    glfwMakeContextCurrent(window);

    init_freetype();
    prepare_text_rendering();

    HANDLE_GL_ERROR(glEnable(GL_DEPTH_TEST));
    create_world_basis();

    while (!main_loop_quit)
    {
        glfwPollEvents();

        if (need_redraw) {
            need_redraw = 0;

            HANDLE_GL_ERROR(glClearColor(0.6, 0.9, 0.9, 1.0));
            HANDLE_GL_ERROR(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

            d3v_scene_draw();

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
