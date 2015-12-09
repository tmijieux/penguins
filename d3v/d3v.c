#include <d3v.h>
#include <d3v/d3v_internal.h>

#include <d3v/scene.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <GL/freeglut.h>

#define WINDOW_TITLE "Penguin"
#define WINDOW_POSITION_X 200
#define WINDOW_POSITION_Y 200

#define HEIGHT  700
#define WIDTH   700


void idle (void)
{

}

void visible (int v)
{
    if (v == GLUT_VISIBLE)
	glutIdleFunc(idle);
    else
	glutIdleFunc(NULL);
}

static void glut_init(void)
{
    int argc;
    glutInit(&argc, NULL);
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutInitWindowPosition(WINDOW_POSITION_X, WINDOW_POSITION_Y);
    glutCreateWindow("penguin1334");
    
    glutDisplayFunc(d3v_scene_draw);
    glutVisibilityFunc(visible);
    glutIdleFunc(NULL);
    glutSpecialFunc(d3v_special_input);
    glutKeyboardFunc(d3v_key);
    glutMouseFunc(d3v_button);
    glutMotionFunc(d3v_mouse_motion);
    glutReshapeFunc(d3v_reshape);
}

/**
 *  Initialisation du module OpenGL.
 */
static void opengl_init(void)
{
    //Initialisation de l'etat d'OpenGL
    //glClearColor(0.6, 0.9, 0.9, 1.0);
    glClearColor(.5, .5, .5, 1.);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glColor3f(1.0, 1.0, 1.0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_DIFFUSE);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(30.0, 1.0, 0.1, 100.0);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    
    //glShadeModel(GL_SMOOTH);
    //glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

    // Parametrage du materiau
    // glEnable(GL_LIGHTING);

    //glPolygonMode(GL_FRONT, GL_FILL);


    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
}

void d3v_exit_main_loop(void)
{
    glutLeaveMainLoop();
}

void d3v_post_redisplay(void)
{
    glutPostRedisplay();
}

// PUBLIC
int d3v_init(int object_count_clue)
{
    glut_init();
    opengl_init();
    d3v_scene_init(object_count_clue);
    return 0;
}

int d3v_start(vec3 *pos)
{
    d3v_scene_start(pos);
    glutMainLoop();
    if (scene.exit_callback)
	scene.exit_callback();
    return 0;
}

int d3v_exit(void)
{
    d3v_scene_exit();

    return 0;
}
