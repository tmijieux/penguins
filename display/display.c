/**
 * @file display.c
 */

#include <stdio.h>
#include <stdlib.h>
#define __USE_XOPEN 1
#include <math.h>
#include <string.h>
#include <pthread.h>

#include <server/path.h>
#include <utils/vec.h>
#include <utils/math.h>
#include <utils/list.h>

#include <display/display.h>

/**
 * Gestion du thread.
 */
static struct {
    pthread_t t;
    pthread_attr_t attr;
    pthread_mutex_t m;
} th;

/**
 * Gestion de la scène.
 */
static struct {
    int key;
    // variables de la souris
    int button, xold, yold;    

    // Generique: Scene
    struct camera *cam;
    struct light *light;
    vec3 centroid;
    
    //Plateau
    struct dtile **tiles;
    int tile_count;

    struct dpenguin **penguins;
    int nb_peng_alloc;
    int penguin_count;

    struct model *penguin_model;
    struct texture **penguin_tex;
    int penguin_tex_count;

    // Affichage Direction
    int linked;
    int **link;
    int activeLink;

    // move's list/queue
    struct record *rec;
    int autoplay;


    struct list *tex_list;
    struct list *mod_list;
} scene;

/**
 * Gestion du déroulement de la lecture de la partie.
 */
enum SENS {
    FORWARD,
    REWIND
};

/**
 * Lis et programme les animations à effectuer.
 * @param s - Sens de la lecture.
 */
static void display_compute_move(enum SENS s)
{
    int tileSrc = (s == FORWARD) ? record_get_current_src(scene.rec)
	: record_get_rewind_src(scene.rec);
    int tileDest = (s == FORWARD) ? record_get_current_dest(scene.rec)
	: record_get_rewind_dest(scene.rec);

    int setPenguin = (tileSrc == -1);
    if (setPenguin)
	tileSrc = tileDest;
    int penguin = -1;
    // Recherche du penguin à bouger
    for (int i = 0; i < scene.penguin_count; i++)
	if (scene.penguins[i] != NULL &&
	    dpenguin_is_on_tile(scene.penguins[i], scene.tiles[tileSrc]))
	    penguin = i;

    if (!setPenguin)
	setPenguin = (tileDest == -1 && s == REWIND);
    if (setPenguin && penguin != -1) {
	if(anim_prepare()){
	    anim_new_movement(scene.penguins[penguin],0,1);
	    anim_set_hide(s != FORWARD);
	    anim_push_movement();
	    anim_launch();
	}
    } else {
	// Calculer le déplacement
	if (anim_prepare()) {
	    if (penguin != -1) {
		vec3 tile_pos; vec3 pen_pos;
		dtile_get_position(scene.tiles[tileDest], &tile_pos);
		dpenguin_get_position(scene.penguins[penguin], &pen_pos);
		float angle = angle_rotation_pingouin(&pen_pos, &tile_pos);
		int nb_move = NB_MOVE;
		if(angle <= 360)
		    nb_move = NB_MOVE / 2;
		
		anim_new_movement(scene.penguins[penguin],0,nb_move);
		if (angle > 360)
		    anim_set_translation(tile_pos);
		anim_set_rotation(angle);
		
		if (angle <= 360){
		    anim_push_movement();		
		    anim_new_movement(scene.penguins[penguin],0,NB_MOVE);
		    anim_set_translation(tile_pos);
		}
		anim_push_movement();
	    }
	    int tile = -1;
	    if (s == FORWARD)
		tile = tileSrc;
	    else
	        tile = tileDest;
	    anim_new_movement(scene.tiles[tile],1,1);
	    anim_set_hide(s == FORWARD);
	    anim_push_movement();
	    anim_launch();
	}
    }
}

/**
 * Lance la lecture d'un mouvement dans l'historique.
 * @param s - Sens de la lecture.
 */
static void display_read_move(enum SENS s)
{
    int newMove = 0;
    if (s == FORWARD)
	newMove = record_next(scene.rec);
    else if (s == REWIND)
	newMove = record_previous(scene.rec);
    if (newMove)
	display_compute_move(s);
}

/******************************************************/
/********* EVENT HANDLING *****************************/

static void display_thread_exit(void);

/**
 * Gestion des évènements clavier.
 * @param key - Code de la touche pressée.
 * @param x - Position x de la sourie.
 * @param y - Position y de la sourie.
 */
static void key_input(unsigned char key, int x, int y)
{
    switch (key) {
    case 'p':
	scene.autoplay = !scene.autoplay;
	break;
    case 'r':
	camera_set_look(scene.cam, scene.centroid);
	camera_set_rotate(scene.cam, -90, 0);
	camera_set_distance(scene.cam, 10.);
	break;
	//case 13: /* touche Enter */
	//break;
    case 27:			/* touche ESC */
	glutLeaveMainLoop();
	glutDestroyWindow(scene.key);
	display_thread_exit();
	break;
    case '5':
	camera_switch_ortho(scene.cam);
	break;
    }
}

/**
 * Gestion des évènements speciaux.
 * @param key - Code de la touche pressée.
 * @param x - Position x de la sourie.
 * @param y - Position y de la sourie.
 */
static void special_input(int key, int x, int y)
{
    switch (key) {
    case GLUT_KEY_UP:
	if (scene.activeLink < scene.tile_count - 1)
	    ++scene.activeLink;
	break;
    case GLUT_KEY_DOWN:
	if (scene.activeLink > 0)
	    --scene.activeLink;
	break;
    case GLUT_KEY_LEFT:
	if(!anim_run())
	    display_read_move(REWIND);
	break;
    case GLUT_KEY_RIGHT:
	if(!anim_run())
	    display_read_move(FORWARD);
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
void mouse(int button, int state, int x, int y)
{
    scene.xold = x; scene.yold = y;
    switch (button) {
    case GLUT_LEFT_BUTTON:
	switch (state) {
	case GLUT_DOWN: scene.button = 1; break;
	case GLUT_UP:
	    scene.button = 0;
	    vec3 pos;
	    mouse_projection(&pos, x, y);
	    printf("%f, %f, %f\n",
		   pos.x, pos.y, pos.z);
	    break;
	} break;
    case GLUT_RIGHT_BUTTON:
	switch (state) {
	case GLUT_DOWN: scene.button = 2; break;
	case GLUT_UP:   scene.button = 0; break;
	} break;
    case 3: camera_add_distance(scene.cam, -0.4); break;
    case 4: camera_add_distance(scene.cam, 0.4);  break;
    }
    glutPostRedisplay();
}

/**
 * Gestion des évènements de mouvement de la souris.
 * @param x - Position x de la sourie.
 * @param y - Position y de la sourie.
 */
static void mousemotion(int x, int y)
{
    if (scene.button == 1) { // bouton gauche
	camera_translate(scene.cam,
			 (double) (scene.xold-x) / 70.,
			 (double) (y-scene.yold) / 70.);
    } else if (scene.button == 2) { // bouton droit
	camera_rotate(scene.cam, scene.yold-y, scene.xold-x);
    }
    scene.xold = x; scene.yold = y;
    glutPostRedisplay();
}

/*******************************************************/
/********************* MISC ***************************/

/**
 * Callback quand la taille de la fenêtre est modifiée.
 * @param w - Largeur de la fenêtre.
 * @param h - Hauteur de la fenêtre.
 */
static void reshape(int w, int h)
{
    if (w > h)
	glViewport((w-h)/2, 0, h, h);
    else
	glViewport(0, (h-w)/2, w, w);
    glutPostRedisplay();
}

/*******************************************************/
/************ DRAWING METHODS **************************/

/**
 * Dessine les bases : (O, x,y,z)
 */
static void draw_basis(void)
{
    glColor3f(1.0, 1.0, 1.0);
    glNormal3f(0,1,0);
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
}

/**
 * Dessine les lignes droites valides à partir d'une tuile pour 
 * atteindre les autres tuiles.
 */
static void display_draw_link(void)
{
    if (!scene.linked)
	return;
    glColor3f(1., 0., 0.);
    glNormal3f(0., 1., 0.);
    glBegin(GL_LINES);
    vec3 pos; dtile_get_position(scene.tiles[scene.activeLink], &pos);
    for (int j = 0; j < scene.tile_count; j++) {
	if (scene.link[scene.activeLink][j]) {
	    vec3 pos2; dtile_get_position(scene.tiles[j], &pos2);
	    glVertex3f(pos.x, pos.y + 0.2, pos.z);
	    glVertex3f(pos2.x, pos2.y + 0.2, pos2.z);
	}
    }
    glEnd();
    glColor3f(1., 1., 1.); // reset color
}

/**
 * Affiche la scène.
 * Mise à jour de la lumière. 
 * Dessin des liens, des tuiles et des penguins.
 */
static void display(void)
{
    int nextMove = !anim_run();
    if (scene.autoplay)
	if(nextMove)
	    display_read_move(FORWARD);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    camera_update(scene.cam);
    light_update(scene.light);
    
    display_draw_link();
    draw_basis();

    for (int i = 0; i < scene.tile_count; i++)
	dtile_draw(scene.tiles[i]);
    for (int i = 0; i < scene.nb_peng_alloc; i++)
	dpenguin_draw(scene.penguins[i]);

    glutSwapBuffers();
    glutPostRedisplay();
}

/***********************************************************/
/******* INITIALIZATION AND FREES **************************/

/*** INITS *****/
/**
 * Initialisation du module Glut.
 * @param argc - Nombre d'arguments.
 * @param argv - Arguments.
 */
static void display_glut_init(int *argc, char *argv[])
{
    glutInit(argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowPosition(WINDOW_POSITION_X, WINDOW_POSITION_Y);
    glutInitWindowSize(WIDTH, HEIGHT);
    scene.key = glutCreateWindow(WINDOW_TITLE);
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE,
		  GLUT_ACTION_GLUTMAINLOOP_RETURNS);
    /** callbacks **/
    glutDisplayFunc(&display);
    glutKeyboardFunc(&key_input);
    glutSpecialFunc(&special_input);
    glutMouseFunc(&mouse);
    glutMotionFunc(&mousemotion);
    glutReshapeFunc(&reshape);
}

/**
 *  Initialisation du module OpenGL.
 */
static void display_opengl_init(void)
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
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

/**
 * Initialisation des propriétés de la scène. Non lié à la partie.
 */
static void display_scene_init(void)
{
    scene.cam = camera_create((vec3) {0.}, 10., -90, 0, 0);
    scene.light = light_create();
    
    scene.penguin_model = model_load_wavefront(PENGUIN_FILE);
    scene.penguin_tex = malloc(sizeof(*scene.penguin_tex) * 10);
    scene.penguin_tex[0] = texture_load("textures/penguin_black.jpg");
    scene.penguin_tex[1] = texture_load("textures/penguin_red.jpg");
    scene.penguin_tex[2] = texture_load("textures/penguin_green.jpg");
    scene.penguin_tex[3] = texture_load("textures/penguin_blue.jpg");
    scene.penguin_tex_count = 4;

    scene.tex_list = list_create(LIST_DEFAULT__);
    scene.mod_list = list_create(LIST_DEFAULT__);
}

/**
 * Initialisation des propriétés de la scène. Lié à la partie.
 * @param tile_count - Nombre de tuiles.
 * @param penguin_count - Nombre de pingouins.
 */
static void display_game_init(int tile_count, int penguin_count)
{
    anim_init();
    scene.linked = 0; scene.activeLink = 0;
    scene.link = malloc(tile_count * sizeof(*scene.link));
    for (int i = 0; i < tile_count; ++i)
	scene.link[i] = calloc(tile_count, sizeof(*scene.link[0]));
    
    scene.tile_count = tile_count;
    scene.penguin_count = penguin_count;

    scene.autoplay = 0;
    scene.rec = record_create(tile_count + (penguin_count * 2));
    // *2 -> marge
    scene.tiles = calloc(tile_count, sizeof(*scene.tiles));
    scene.penguins = calloc(penguin_count, sizeof(*scene.penguins));

    scene.nb_peng_alloc = 0;
    scene.button = 0;
    scene.xold = 0; scene.yold = 0;
}

/**
 * Initialisation du module d'affichage.
 * @param tile_count - Nombre de tuiles.
 * @param penguin_count - Nombre de pingouins.
 */
void display_init(int tile_count, int penguin_count)
{
    int argc = 0;
    display_glut_init(&argc, NULL);
    display_opengl_init();
    display_scene_init();
    display_game_init(tile_count, penguin_count);
}

/*** STARTS *****/
/**
 * Démarrer la boucle d'affichage.
 * @param ARGS - Arguments.
 */
static void *display_thread_start(void *ARGS)
{
    /** Entre dans la boucle principale glut **/
    glutMainLoop();
    pthread_exit(DISPLAY_THREAD_RETVAL);
}

/**
 * Démarrer le thread d'affichage.
 */
void display_start(void)
{
    camera_set_look(scene.cam, scene.centroid);
    pthread_attr_init(&th.attr);
    pthread_create(&th.t, &th.attr, &display_thread_start, NULL);
}

/*** EXITS *****/
/**
 * Libération de la mémoire utilisée avant de quitter.
 */
static void display_scene_exit(void)
{
    camera_free(scene.cam);
    light_free(scene.light);
    
    for (int i = 0; i < scene.penguin_tex_count; i++)
	texture_free(scene.penguin_tex[i]);
    free(scene.penguin_tex);

    //printf("free penguin model\n");
    model_free(scene.penguin_model);

    while (list_size(scene.tex_list)>0) {
	texture_free(list_get_element(scene.tex_list, 1));
	list_remove_element(scene.tex_list, 1);
    }
    list_destroy(scene.tex_list);
    
    while (list_size(scene.mod_list)>0) {
	model_free(list_get_element(scene.mod_list, 1));
	list_remove_element(scene.mod_list, 1);
    }
    list_destroy(scene.mod_list);
}

/**
 * Lancement de la procédure de fermeture du module d'affichage.
 */
static void display_game_exit(void)
{
    for (int i = 0; i < scene.penguin_count; i++)
	dpenguin_free(scene.penguins[i]);
    free(scene.penguins);
    
    for (int i = 0; i < scene.tile_count; i++)
	dtile_free(scene.tiles[i]);
    free(scene.tiles);
    
    record_free(scene.rec);
    for (int i = 0; i < scene.tile_count; ++i)
	free(scene.link[i]);
    free(scene.link);
}

/**
 * Arrêt du thread d'affichage.
 */
static void display_thread_exit(void)
{
    display_scene_exit();
    display_game_exit();
}

/**
 * Fermeture de l'affichage.
 */
void display_exit(void)
{
    void *retval = NULL;
    puts("Waiting for the user to end the display ...");
    pthread_join(th.t, &retval);
    if (retval == DISPLAY_THREAD_RETVAL)
	puts("display thread exited successfully");
}

/************************************************************/
/************ ADD ELEMENTS TO SCENE *************************/

/**
 * Permet au créateur de carte d'ajouter des tuiles à l'affichage.
 * @param id - Identifiant de la tuile.
 * @param m - Le model 3D de la tuile.
 * @param t - La texture de la tuile.
 * @param posx - Position sur l'axe des x de la tuile.
 * @param posy - Position sur l'axe des y de la tuile.
 * @param posz - Position sur l'axe des z de la tuile.
 * @param angle - Enrientation de la tuile par rapport à
 *            l'axe des y. (En degré)
 * @param scale - 0.5 réduit le model par 2
 *                  3. augmente le model par 3
 *                  1. Ne fais rien.
 * @param fish_count - Nombre de poissons sur la tuile.
 * @return int - 1 si l'ajout est valide.
 */
int display_add_tile(int id, struct model *m, struct texture *t,
		     double posx, double posy, double posz,
		     int angle, float scale, int fish_count)
{
    // set camera at centroid of tiles
    static int weight = 0.;
    static vec3 pos = {0.,0.,0.};
    pos.x = (weight * pos.x + posx) / (weight+1);
    pos.y = (weight * pos.y + posy) / (weight+1);
    pos.z = (weight * pos.z + posz) / (weight+1);
    weight ++;
    scene.centroid = pos;
    
    if (id >= 0 && id < scene.tile_count) {
	vec3 pos = { posx, posy, posz};
	scene.tiles[id] = dtile_create(m, t, pos, (double)angle,
				       (double)scale, fish_count);
	if (scene.tiles[id] == NULL) {
	    printf("display_add_tile failed\n");
	    exit(EXIT_FAILURE);
	}
    }
    return 0;
}

/**
 * Permet au créateur de carte d'ajouter des pingouins à l'affichage.
 * @param tile - Tuile où se trouve le pingouin.
 * @param player - Identifiant du joueur.
 * @return int - 1 si l'ajout est valide.
 */
int display_add_penguin(int tile, int player)
{
    if (scene.nb_peng_alloc < scene.penguin_count) {
	int texid = player % scene.penguin_tex_count;
	struct texture *t = scene.penguin_tex[texid];
	vec3 pos; dtile_get_position(scene.tiles[tile], &pos);
	scene.penguins[scene.nb_peng_alloc]
	    = dpenguin_create(scene.penguin_model, t, pos, 0., 0.1);
	if (scene.penguins[scene.nb_peng_alloc] == NULL)
	    return 0;
	
	dpenguin_set_position(scene.penguins[scene.nb_peng_alloc], pos);
	dpenguin_hide(scene.penguins[scene.nb_peng_alloc]);
	
	scene.nb_peng_alloc++;
	record_add(scene.rec, -1, tile);
	return 1;
    }
    return 0;
}

/**
 * Permet au serveur d'ajouter des mouvement dans l'historique de la 
 * partie.
 * @param src - Tuile d'origine du mouvement.
 * @param dst - Tuile de destination du mouvement.
 * @return int - 1 si l'enregistrement est valide.
 */
int display_add_move(int src, int dst)
{
    return record_add(scene.rec, src, dst);
}

/**
 * Permet au serveur d'ajouter des lignes droites à l'affichage.
 * @param src - Tuile d'origine de la ligne.
 * @param dst - Tuile de destination de la ligne.
 */
void display_add_link(int src, int dst)
{
    scene.linked = 1;
    scene.link[src][dst] = 1;
}

/**
 * Permet au créateur de carte de passer un pointer au module
 * d'affichage, donc le thread où l'OpenGL est exécuté peut 
 * en toute sécurité libérer la mémoire associée à la texture 
 * avant que le thread et le contexte de l'OpenGl termine.
 */
void display_register_texture(struct texture *t)
{
    list_add_element(scene.tex_list, t);
}

/**
 * Permet au créateur de carte de passer un pointer au module
 * d'affichage, donc le thread où l'OpenGL est exécuté peut 
 * en toute sécurité libérer la mémoire associée au modèle 
 * avant que le thread et le contexte de l'OpenGl termine.
 */
void display_register_model(struct model *m)
{
    list_add_element(scene.mod_list, m);
}
