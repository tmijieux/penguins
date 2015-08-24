#include <d3v.h>
#include <d3v/scene.h>

/**
 * Initialisation du module Glut.
 * @param argc - Nombre d'arguments.
 * @param argv - Arguments.
 */
static void glut_init(int *argc, char *argv[])
{
    glutInit(argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowPosition(WINDOW_POSITION_X, WINDOW_POSITION_Y);
    glutInitWindowSize(WIDTH, HEIGHT);
    scene.key = glutCreateWindow(WINDOW_TITLE);
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE,
		  GLUT_ACTION_GLUTMAINLOOP_RETURNS);
    /** callbacks **/
    d3v_init_glut_callback();
}

/**
 *  Initialisation du module OpenGL.
 */
static void opengl_init(void)
{
    //Initialisation de l'etat d'OpenGL
    glClearColor(0.6, 0.9, 0.9, 1.0);
    glColor3f(1.0, 1.0, 1.0);

    glShadeModel(GL_SMOOTH);
    //glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

    // Parametrage du materiau
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);

    glColorMaterial(GL_FRONT, GL_DIFFUSE);
    glPolygonMode(GL_FRONT, GL_FILL);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// PUBLIC

int d3v_init(int object_count_clue)
{
    int argc = 0;
    glut_init(&argc, NULL);
    opengl_init();
    d3v_scene_init(object_count_clue);
    return 0;
}

int d3v_start(void)
{
    glutMainLoop();
    return 0;
}

int d3v_exit(void)
{
    d3v_scene_exit();
    return 0;
}

