/**
 * @file display.c
 */
#define __USE_XOPEN 1

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "penguins_opengl.h"

#include "utils/vec.h"

#include "d3v/d3v.h"
#include "d3v/shader.h"
#include "d3v/user_callback.h"
#include "d3v/scene.h"
#include "d3v/mouse_projection.h"
#include "d3v/camera.h"
#include "d3v/light.h"
#include "d3v/object.h"
#include "d3v/d3v_internal.h"



/**
 * Gestion de la scène.
 */
scene_t scene;

/******************************************************/
/********* EVENT HANDLING *****************************/

/**
 * Gestion des évènements clavier.
 * @param key - Code de la touche pressée.
 * @param x - Position x de la sourie.
 * @param y - Position y de la sourie.
 */
__internal
void d3v_key(int key, int scancode, int action, int mods, int x, int y)
{
    if (scene.key_input_callback) {
        scene.key_input_callback(key, scancode, action, mods, x, y);
    }
    switch (key) {
        case GLFW_KEY_R: // 'r'
            d3v_camera_set_look(scene.camera, &scene.first_look);
            d3v_camera_set_rotate(scene.camera, -90, -90);
            d3v_camera_set_distance(scene.camera, 10.);
            break;
        case GLFW_KEY_ESCAPE: // ESC
            d3v_request_exit_from_main_loop(); // maybe remove that
            break;
        case GLFW_KEY_5: // 5 (with or without SHIFT key)
        case GLFW_KEY_KP_5: // 'KP_5'
            d3v_camera_switch_ortho(scene.camera);
            break;
    }
}

/**
 * Gestion des évènements bouton de la souris.
 * @param button - Code du bouton générant l'évènement.
 * @param state - Etat du bonton.
 * @param x - Position x de la sourie.
 * @param y - Position y de la sourie.
 */

__internal
void d3v_button(int button, int state, int mods, int x, int y)
{
    scene.xold = x;
    scene.yold = y;

    if (scene.mouse_callback) {
        scene.mouse_callback(button, state, mods, x, y);
    }

    switch (button) {
        case GLFW_MOUSE_BUTTON_1:
            switch (state) {
                case GLFW_PRESS:   scene.button = 1; break;
                case GLFW_RELEASE: scene.button = 0; break;
            }
            break;
        case GLFW_MOUSE_BUTTON_2:
            switch (state) {
                case GLFW_PRESS:   scene.button = 2; break;
                case GLFW_RELEASE: scene.button = 0; break;
            }
            break;
    }
}

/**
 * Gestion des évènements de mouvement de la souris.
 * @param x - Position x de la sourie.
 * @param y - Position y de la sourie.
 */
__internal
void d3v_mouse_motion(int x, int y)
{
    if (scene.button == 1) { // bouton gauche
        d3v_camera_translate(scene.camera,
                             (double) (scene.xold-x) / 70.,
                             (double) (y-scene.yold) / 70.);
    } else if (scene.button == 2) { // bouton droit
        d3v_camera_rotate(scene.camera, scene.yold-y, scene.xold-x);
    }
    scene.xold = x; scene.yold = y;
}

/**
 * Callback quand la taille de la fenêtre est modifiée.
 * @param w - Largeur de la fenêtre.
 * @param h - Hauteur de la fenêtre.
 */
__internal
void d3v_reshape(int width, int height)
{
    /* if (w > h) { */
    /*     glViewport((w-h)/2, 0, h, h); */
    /* } else { */
    /*     glViewport(0, (h-w)/2, w, w); */
    /* } */
    glViewport(0, 0, width, height);
    d3v_camera_set_aspect_ratio(scene.camera, (double)width / (double)height);
//    scene.camera
    // need to recompute perspective projection
    // or ensure aspect ratio cannot be changed
    // because perspective depend on aspect ratio
    // also if perspective does not match window
    // this can potentially create wrong results
    // for unproject ??? (mouseclick feature)
}

/*******************************************************/
/************ DRAWING METHODS **************************/

static void draw_camera(int shader_id, void *data)
{
    d3v_camera_draw(scene.camera, shader_id);
}


/**
 * Affiche la scène.
 * Mise à jour de la lumière.
 * Dessin des liens, des tuiles et des penguins.
 */
__internal
void d3v_scene_draw()
{
    int need_draw = d3v_camera_update(scene.camera);
    if (need_draw) {
        for_each_shader_program(draw_camera, NULL);
    }

    // all lighting is currently
    // hardcoded in shader code
    // consider adding uniform to do materials...
    /* d3v_light_update(scene.light); */

    if (scene.draw_callback) {
        scene.draw_callback();
    }

    for (int i = 0; i < scene.object_count; i++)
    {
        object_t * obj = scene.object_buf[i];
        d3v_object_draw(obj);
    }
}

/***********************************************************/
/******* INITIALIZATION AND FREES **************************/

/*** INITS *****/

/**
 * Initialisation des propriétés de la scène. Non lié à la partie.
 */
__internal
void d3v_scene_module_init(int obj_count_clue)
{
    scene.camera = d3v_camera_create((vec3) {0.0 ,0.0, 0.0}, 10., -90.0, -90.0, 0);
    scene.light = d3v_light_create();

    scene.object_count = 0;
    scene.object_buf = calloc(sizeof(*scene.object_buf), obj_count_clue);
    scene.button = 0;
    scene.xold = 0;
    scene.yold = 0;
}

__internal
void d3v_scene_module_exit(void)
{
    d3v_camera_free(scene.camera);
    d3v_light_free(scene.light);

    int count = scene.object_count;
    scene.object_count = 0;
    for (int i = 0; i < count ; ++i) {
        d3v_object_free(scene.object_buf[i]);
        scene.object_buf[i] = NULL;
    }
    free(scene.object_buf);
    scene.object_buf = NULL;
}


// PUBLIC
void d3v_set_mouse_callback(mouse_callback_t mouse_callback)
{
    scene.mouse_callback = mouse_callback;
}

void d3v_set_key_input_callback( key_input_callback_t key_input_callback)
{
    scene.key_input_callback = key_input_callback;
}

void d3v_set_draw_callback(draw_callback_t draw_callback)
{
    scene.draw_callback = draw_callback;
}

void d3v_add_object(object_t *obj)
{
    scene.object_buf[scene.object_count++] = obj;
}


