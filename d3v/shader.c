#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>

#include "d3v/d3v.h"
#include "d3v/shader.h"


static int shader_initialized[PENGUIN_NUM_SHADER] = {0};
static int shader_programs[PENGUIN_NUM_SHADER];

char* from_file(const char *path);
int load_shader_from_source(char *vertex_shader_source,char *fragment_shader_source);

int get_or_load_shader(ShaderId shader_id)
{
    if (shader_id < PENGUIN_MIN_SHADER
        || shader_id > PENGUIN_MAX_SHADER) {
        fprintf(stderr, "Invalid shader required `%d`\n", shader_id);
        exit(EXIT_FAILURE);
    }
    if (shader_initialized[shader_id]) {
        return shader_programs[shader_id];
    }

    int shader_program = -1;
    switch (shader_id) {
    case PENGUIN_SHADER_TEX_LIGHT:
        shader_program = load_shader_from_source(
            from_file("shaders/VERTEX_texture_light.glsl"),
            from_file("shaders/FRAGMENT_texture_light.glsl")
        );
        break;
    case PENGUIN_SHADER_SIMPLE_RED:
        shader_program = load_shader_from_source(
            from_file("shaders/VERTEX_simple.glsl"),
            from_file("shaders/FRAGMENT_red.glsl")
        );
        break;
    case PENGUIN_SHADER_COLOR_NO_MODEL:
        shader_program = load_shader_from_source(
            from_file("shaders/VERTEX_color_no_model.glsl"),
            from_file("shaders/FRAGMENT_color.glsl")
        );
        break;
    case PENGUIN_SHADER_COLOR_NO_PROJ:
        shader_program = load_shader_from_source(
            from_file("shaders/VERTEX_color_no_proj.glsl"),
            from_file("shaders/FRAGMENT_color.glsl")
        );
        break;
    case PENGUIN_SHADER_TEXT:
        shader_program = load_shader_from_source(
            from_file("shaders/VERTEX_text.glsl"),
            from_file("shaders/FRAGMENT_text.glsl")
        );
        break;
    default:
        break;
    }
    if (shader_program < 0) {
        return -1;
    }

    shader_programs[shader_id] = shader_program;
    shader_initialized[shader_id] = 1;
    return shader_program;
}

char* from_file(const char *path)
{
    FILE *f = fopen(path, "rb");
    if (!f) {
        fprintf(stderr, "Failed to open shader `%s`: %s\n", path, strerror(errno));
        exit(EXIT_FAILURE);
    }
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);  /* same as rewind(f); */

    char *data = malloc(fsize + 1);
    fread(data, fsize, 1, f);
    fclose(f);

    data[fsize] = 0;

    return data;
}

int load_shader_from_source(char *vertex_shader_source,char *fragment_shader_source)
{
    uint32_t vertex_shader, fragment_shader;
    int success;
    char infoLog[512] = "";

    typedef const char*const* SOURCE;

    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, (SOURCE)&vertex_shader_source, NULL);
    glCompileShader(vertex_shader);
    free(vertex_shader_source);

    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex_shader, sizeof(infoLog), NULL, infoLog);
        fprintf(stderr, "Error for vertex shader compilation: %s\n", infoLog);
        exit(EXIT_FAILURE);
    }

    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, (SOURCE)&fragment_shader_source, NULL);
    glCompileShader(fragment_shader);
    free(fragment_shader_source);

    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        memset(infoLog, 0, sizeof(infoLog));
        glGetShaderInfoLog(fragment_shader, sizeof(infoLog), NULL, infoLog);
        fprintf(stderr, "Error for fragment shader compilation: %s\n", infoLog);
        exit(EXIT_FAILURE);
    }

    uint32_t shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);


    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success) {
        memset(infoLog, 0, sizeof(infoLog));
        glGetProgramInfoLog(shader_program, sizeof(infoLog), NULL, infoLog);
        fprintf(stderr, "Error for  shader program linkage: %s\n", infoLog);

        exit(EXIT_FAILURE);
    }
    glUseProgram(shader_program);
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return shader_program;
}



void for_each_shader_program(shader_callback_t callback, void *data)
{
    for (int i = PENGUIN_MIN_SHADER; i <= PENGUIN_MAX_SHADER; ++i) {
        if (shader_initialized[i]) {
            callback(shader_programs[i], data);
        }
    }
}
