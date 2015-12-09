/**
 * @file display.c
 */

#include <stdio.h>
#include <stdlib.h>
#define __USE_XOPEN 1
#include <math.h>
#include <string.h>
#include <pthread.h>

#include <GL/glu.h>

#include <utils/vec.h>

#include <d3v/user_callback.h>
#include <d3v/scene.h>
#include <d3v/mouse_projection.h>
#include <d3v/camera.h>
#include <d3v/light.h>
#include <d3v/object.h>

#include <d3v/d3v_internal.h>


#define ERROR      printf("error: %s\n\n", \
			  gluErrorString(glGetError()))


/**
 * Gestion de la scène.
 */
struct scene scene __internal;

/******************************************************/
/********* EVENT HANDLING *****************************/

/**
 * Gestion des évènements clavier.
 * @param key - Code de la touche pressée.
 * @param x - Position x de la sourie.
 * @param y - Position y de la sourie.
 */
__internal
void d3v_key(unsigned char key, int x, int y) 
{
    if (scene.key_input_callback)
	scene.key_input_callback(key, x, y);
    
    switch (key) {
    case 27: // 'r'
	d3v_camera_set_look(scene.cam, &scene.first_look);
	d3v_camera_set_rotate(scene.cam, -90, 0);
	d3v_camera_set_distance(scene.cam, 10.);
	break;
    case 9:	// ESC
	if (scene.exit_callback)
	    scene.exit_callback();
	d3v_exit_main_loop(); // maybe remove that
	break;
    case 84: // 'KP_5'
	d3v_camera_switch_ortho(scene.cam);
	break;
    }
}

/**
 * Gestion des évènements speciaux.
 * @param key - Code de la touche pressée.
 * @param x - Position x de la sourie.
 * @param y - Position y de la sourie.
 */
__internal
void d3v_special_input(int key, int x, int y)
{
    if (scene.spe_input_callback)
	scene.spe_input_callback(key, x, y);
}

/**
 * Gestion des évènements bouton de la souris.
 * @param button - Code du bouton générant l'évènement.
 * @param state - Etat du bonton.
 * @param x - Position x de la sourie.
 * @param y - Position y de la sourie.
 */

__internal
void d3v_button(int button, int state, int x, int y)
{
    scene.xold = x;
    scene.yold = y;

    if (scene.mouse_callback)
	scene.mouse_callback(button, state, x, y);

    switch (button) {
    case Button1:
	switch (state) {
	case ButtonPress:   scene.button = 1; break;
	case ButtonRelease: scene.button = 0; break;
	} break;
    case Button3:
	switch (state) {
	case ButtonPress:   scene.button = 2; break;
	case ButtonRelease: scene.button = 0; break;
	} break;
    case Button4: d3v_camera_add_distance(scene.cam, -0.4); break;
    case Button5: d3v_camera_add_distance(scene.cam, 0.4);  break;
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
	d3v_camera_translate(scene.cam,
			 (double) (scene.xold-x) / 70.,
			 (double) (y-scene.yold) / 70.);
    } else if (scene.button == 2) { // bouton droit
	d3v_camera_rotate(scene.cam, scene.yold-y, scene.xold-x);
    }
    scene.xold = x; scene.yold = y;
}

/**
 * Callback quand la taille de la fenêtre est modifiée.
 * @param w - Largeur de la fenêtre.
 * @param h - Hauteur de la fenêtre.
 */
__internal
void d3v_reshape(int w, int h)
{
    if (w > h)
       glViewport((w-h)/2, 0, h, h);
    else
       glViewport(0, (h-w)/2, w, w);
}

/*******************************************************/
/************ DRAWING METHODS **************************/

/**
 * Dessine les bases : (O, x,y,z)
 */
static void draw_basis(void)
{
    GLboolean b; // save lighting state and disable
    glGetBooleanv(GL_LIGHTING, &b);
    glDisable(GL_LIGHTING);

    glColor3f(1.0, 1.0, 1.0);
        
    glNormal3f(0, 1, 0);
    glBegin(GL_LINES);
    glColor3f(1., 0., 0.);    // x red
    glVertex3f(0., 0., 0.);
    glVertex3f(1., 0., 0.);

    glColor3f(.0, 1., 0.);    // y green
    glVertex3f(0., 0., 0.);
    glVertex3f(0., 1., 0.);

    glColor3f(0., 0., 1.);    // z blue
    glVertex3f(0., 0., 0.);
    glVertex3f(0., 0., 1.);
    glEnd();
    glColor3f(1.0, 1.0, 1.0); // Reset Color

    if (b) // restore lighting state
	glEnable(GL_LIGHTING);
}

/**
 * Affiche la scène.
 * Mise à jour de la lumière. 
 * Dessin des liens, des tuiles et des penguins.
 */
__internal
void d3v_scene_draw(void)
{
    glXMakeCurrent(display, win, ctx);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    d3v_camera_update(scene.cam);
    d3v_light_update(scene.light);
    
    if (scene.draw_callback)
	scene.draw_callback();
	
    draw_basis(); // TODO: option to disable this
    
    for (int i = 0; i < scene.object_count; i++)
	d3v_object_draw(scene.object_buf[i]);

    // add wire and (raster) string HERE !
    
//    printf("draw\n");
    glXSwapBuffers(display, win);
}

/***********************************************************/
/******* INITIALIZATION AND FREES **************************/

/*** INITS *****/

/**
 * Initialisation des propriétés de la scène. Non lié à la partie.
 */
__internal
void d3v_scene_init(int obj_count_clue)
{
    scene.cam = d3v_camera_create((vec3) {0.}, 10., -90, 0, 0);
    scene.light = d3v_light_create();

    scene.object_count = 0;
    scene.object_buf = calloc(sizeof(*scene.object_buf),
			      obj_count_clue);
    scene.button = 0;
    scene.xold = 0; scene.yold = 0;
}

__internal
void d3v_scene_exit(void)
{
    d3v_camera_free(scene.cam);
    d3v_light_free(scene.light);

    for (int i = 0; i < scene.object_count; ++i)
	d3v_object_free(scene.object_buf[i]);
    free(scene.object_buf);
}

__internal
void d3v_scene_start(vec3 *pos)
{
    scene.first_look = *pos;
    d3v_camera_set_look(scene.cam, pos);
}

// PUBLIC
void d3v_set_mouse_callback(
    void (*mouse_callback)(int,int,int,int))
{
    scene.mouse_callback = mouse_callback;
}

void d3v_set_spe_input_callback(
    void (*spe_input_callback)(int,int,int))
{
    scene.spe_input_callback = spe_input_callback;
}

void d3v_set_key_input_callback(
    void (*key_input_callback)(int,int,int))
{
    scene.key_input_callback = key_input_callback;
}

void d3v_set_draw_callback(
    void (*draw_callback)(void))
{
    scene.draw_callback = draw_callback;
}

void d3v_set_exit_callback(
    void (*exit_callback)(void))
{
    scene.exit_callback = exit_callback;
}


void d3v_add_object(struct object *o)
{
    scene.object_buf[scene.object_count++] = o;
}
