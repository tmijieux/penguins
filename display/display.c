#include <stdio.h>
#include <pthread.h>

#include <display.h>
#include <display/dsp.h>
#include <display/record.h>
#include <display/dtile.h>
#include <display/dpenguin.h>
#include <display/animator.h>

#include <d3v.h>

#include <utils/vec.h>
#include <utils/math.h>
#include <utils/list.h>

#define PENGUIN_FILE "models/penguin.obj"

/**
 * Gestion du thread.
 */
static struct {
    pthread_t t;
    pthread_attr_t attr;
    pthread_mutex_t m;
} th;

struct display dsp;

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
    int tileSrc = (s == FORWARD) ? record_get_current_src(dsp.rec)
	: record_get_rewind_src(dsp.rec);
    int tileDest = (s == FORWARD) ? record_get_current_dest(dsp.rec)
	: record_get_rewind_dest(dsp.rec);

    int setPenguin = (tileSrc == -1);
    if (setPenguin)
	tileSrc = tileDest;

    
    int penguin;
    penguin = dtile_get_penguin(dsp.tiles[tileSrc]);

    if (!setPenguin)
	setPenguin = (tileDest == -1 && s == REWIND);
    if (setPenguin && penguin != -1) {
	if (anim_prepare()) {
	    anim_new_movement(dsp.penguins[penguin], 0, 1);
	    anim_set_hide(s != FORWARD);
	    anim_push_movement();
	    anim_launch();
	}
    } else {
// Calculer le déplacement
	if (anim_prepare()) {
	    if (penguin != -1) {
		vec3 tile_pos; vec3 pen_pos;
		dtile_get_position(dsp.tiles[tileDest], &tile_pos);
		dpenguin_get_position(dsp.penguins[penguin], &pen_pos);
		float angle = angle_rotation_pingouin(&pen_pos, &tile_pos);
		int nb_move = NB_MOVE;
		if(angle <= 360)
		    nb_move = NB_MOVE / 2;
		
		anim_new_movement(dsp.penguins[penguin], 0, nb_move);
		if (angle > 360)
		    anim_set_translation(tile_pos);
		anim_set_rotation(angle);
		
		if (angle <= 360) {
		    anim_push_movement();		
		    anim_new_movement(dsp.penguins[penguin], 0, NB_MOVE);
		    anim_set_translation(tile_pos);
		}
		anim_push_movement();
	    }
	    int tile = -1;
	    if (s == FORWARD)
		tile = tileSrc;
	    else
		tile = tileDest;
	    anim_new_movement(dsp.tiles[tile], 1, 1);
	    anim_set_hide(s == FORWARD);
	    anim_push_movement();
	    anim_launch();
	}
    }
    
    if (tileSrc >= 0 && tileDest >= 0) {
	dtile_set_penguin(dsp.tiles[tileSrc], -1);
	dtile_set_penguin(dsp.tiles[tileDest], penguin);
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
	newMove = record_next(dsp.rec);
    else if (s == REWIND)
	newMove = record_previous(dsp.rec);
    if (newMove)
	display_compute_move(s);
}

/*******************************************************/
/************ DRAWING METHODS **************************/

/**
 * Dessine les lignes droites valides à partir d'une tuile pour 
 * atteindre les autres tuiles.
 */
static void draw_link(void)
{

    // TODO : add support and interface for lines(wire) in d3v
    //        
    // and use it here (or not if user know opengl
    //                       can be used like that)
    
    if (!dsp.linked)
	return;
    glColor3f(1., 0., 0.);
    glNormal3f(0., 1., 0.);
    glBegin(GL_LINES);
    vec3 pos; dtile_get_position(dsp.tiles[dsp.activeLink], &pos);
    for (int j = 0; j < dsp.tile_count; j++) {
	if (dsp.link[dsp.activeLink][j]) {
	    vec3 pos2; dtile_get_position(dsp.tiles[j], &pos2);
	    glVertex3f(pos.x, pos.y + 0.2, pos.z);
	    glVertex3f(pos2.x, pos2.y + 0.2, pos2.z);
	}
    }
    glEnd();
    glColor3f(1., 1., 1.); // reset color
}

static void draw(void)
{
    int nextMove = !anim_run();
    if (dsp.autoplay)
	if(nextMove)
	    display_read_move(FORWARD);
    draw_link();


    for (int i = 0; i < dsp.tile_count; ++i)
	dtile_draw(dsp.tiles[i]);

    for (int i = 0; i < dsp.penguin_count; ++i)
	dpenguin_draw(dsp.penguins[i]);
	
}

/***********************************************************/
/******* EVENTS MANAGEMENT **************************/

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
	if (dsp.activeLink < dsp.tile_count - 1)
	    ++dsp.activeLink;
	break;
    case GLUT_KEY_DOWN:
	if (dsp.activeLink > 0)
	    --dsp.activeLink;
	break;
    case GLUT_KEY_LEFT:
	if (!anim_run())
	    display_read_move(REWIND);
	break;
    case GLUT_KEY_RIGHT:
	if (!anim_run())
	    display_read_move(FORWARD);
	break;
    }
}

static void key_input(int key, int x, int y)
{
    
    switch (key) {
    case 'p':
	dsp.autoplay = !dsp.autoplay;
	
	break;
    }
}

static void mouse(int button, int state, int x, int y)
{
    switch (button) {
    case GLUT_LEFT_BUTTON:
	switch (state) {
	case GLUT_UP:
	    //TODO: implements mouseclick here
	    break;
	}
	break;
    }
}
       

/***********************************************************/
/******* INITIALIZATION AND FREES **************************/

/*** INITS *****/
static void exit_thread(void);

/**
 * Initialisation des propriétés de la scène. Non lié à la partie.
 */
static void init_d3v_stuff(void)
{
    dsp.penguin_model = model_load_wavefront(PENGUIN_FILE);
    dsp.penguin_tex = malloc(sizeof(*dsp.penguin_tex) * 10);
    dsp.penguin_tex[0] = texture_load("textures/penguin_black.jpg");
    dsp.penguin_tex[1] = texture_load("textures/penguin_red.jpg");
    dsp.penguin_tex[2] = texture_load("textures/penguin_green.jpg");
    dsp.penguin_tex[3] = texture_load("textures/penguin_blue.jpg");
    dsp.penguin_tex_count = 4;
        
    dsp.tex_list = list_create(LIST_DEFAULT__);
    dsp.mod_list = list_create(LIST_DEFAULT__);


    d3v_set_draw_callback(&draw);
    d3v_set_key_input_callback(&key_input);
    d3v_set_spe_input_callback(&special_input);
    d3v_set_mouse_callback(&mouse);
    d3v_set_exit_callback(&exit_thread);
}

/**
 * Initialisation des propriétés de la scène. Lié à la partie.
 * @param tile_count - Nombre de tuiles.
 * @param penguin_count - Nombre de pingouins.
 */
static void init_penguin_stuff(int tile_count, int penguin_count)
{
    anim_init();
    dsp.linked = 0; dsp.activeLink = 0;
    dsp.link = malloc(tile_count * sizeof(*dsp.link));
    for (int i = 0; i < tile_count; ++i)
	dsp.link[i] = calloc(tile_count, sizeof(*dsp.link[0]));
    
    dsp.tile_count = tile_count;
    dsp.penguin_count = penguin_count;

    dsp.autoplay = 0;
    dsp.rec = record_create(tile_count + (penguin_count * 2));
// *2 -> marge
    dsp.tiles = calloc(tile_count, sizeof(*dsp.tiles));
    dsp.penguins = calloc(penguin_count, sizeof(*dsp.penguins));

    dsp.nb_peng_alloc = 0;
}

/**
 * Initialisation du module d'affichage.
 * @param tile_count - Nombre de tuiles.
 * @param penguin_count - Nombre de pingouins.
 */
void display_init(int tile_count, int penguin_count)
{
    d3v_init(tile_count + penguin_count);
    init_d3v_stuff();
    init_penguin_stuff(tile_count, penguin_count);
}


/*** STARTS *****/
static void* thread_start(void *args)
{
    d3v_start(&dsp.centroid);
    return NULL;
}

/**
 * Démarrer le thread d'affichage.
 */
void display_start(void)
{
    pthread_attr_init(&th.attr);
    pthread_create(&th.t, &th.attr, &thread_start, NULL);
}

/*** EXITS *****/
/**
 * Libération de la mémoire utilisée avant de quitter.
 */
static void free_d3v_stuff(void)
{
    for (int i = 0; i < dsp.penguin_tex_count; i++)
	texture_free(dsp.penguin_tex[i]);
    free(dsp.penguin_tex);

//printf("free penguin model\n");
    model_free(dsp.penguin_model);

    while (list_size(dsp.tex_list)>0) {
	texture_free(list_get_element(dsp.tex_list, 1));
	list_remove_element(dsp.tex_list, 1);
    }
    list_destroy(dsp.tex_list);
    
    while (list_size(dsp.mod_list)>0) {
	model_free(list_get_element(dsp.mod_list, 1));
	list_remove_element(dsp.mod_list, 1);
    }
    list_destroy(dsp.mod_list);
}

/**
 *
 */
static void free_penguin_stuff(void)
{
    for (int i = 0; i < dsp.penguin_count; i++)
	dpenguin_free(dsp.penguins[i]);
    free(dsp.penguins);
    
    for (int i = 0; i < dsp.tile_count; i++)
	dtile_free(dsp.tiles[i]);
    free(dsp.tiles);
    
    record_free(dsp.rec);
    for (int i = 0; i < dsp.tile_count; ++i)
	free(dsp.link[i]);
    free(dsp.link);
}

/**
 * Arrêt du thread d'affichage.
 */
static void exit_thread(void)
{
    free_penguin_stuff();
    free_d3v_stuff();
    d3v_exit();
    pthread_exit(DISPLAY_THREAD_RETVAL);
}

int display_exit(void)
{
    void *retval = NULL;
    puts("Waiting for the user to end the display ...");
    pthread_join(th.t, &retval);
    if (retval == DISPLAY_THREAD_RETVAL) {
	puts("display thread exited successfully");
	return 0;
    }
    return -1;
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
    dsp.centroid = pos;
    
    if (id >= 0 && id < dsp.tile_count) {
	vec3 pos = { posx, posy, posz};
	dsp.tiles[id] = dtile_create(m, t, pos, (double)angle,
				       (double)scale, fish_count);
	if (dsp.tiles[id] == NULL) {
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
    if (dsp.nb_peng_alloc < dsp.penguin_count) {
	int texid = player % dsp.penguin_tex_count;
	struct texture *t = dsp.penguin_tex[texid];
	vec3 pos; dtile_get_position(dsp.tiles[tile], &pos);
	dsp.penguins[dsp.nb_peng_alloc]
	    = dpenguin_create(dsp.penguin_model, t, pos, 0., 0.1);
	if (dsp.penguins[dsp.nb_peng_alloc] == NULL)
	    return 0;
	dtile_set_penguin(dsp.tiles[tile], dsp.nb_peng_alloc);
	    
	dpenguin_hide(dsp.penguins[dsp.nb_peng_alloc]);
	
	dsp.nb_peng_alloc++;
	record_add(dsp.rec, -1, tile);
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
    return record_add(dsp.rec, src, dst);
}

/**
 * Permet au serveur d'ajouter des lignes droites à l'affichage.
 * @param src - Tuile d'origine de la ligne.
 * @param dst - Tuile de destination de la ligne.
 */
void display_add_link(int src, int dst)
{
    dsp.linked = 1;
    dsp.link[src][dst] = 1;
}

/**
 * Permet au créateur de carte de passer un pointer au module
 * d'affichage, donc le thread où l'OpenGL est exécuté peut 
 * en toute sécurité libérer la mémoire associée à la texture 
 * avant que le thread et le contexte de l'OpenGl termine.
 */
void display_register_texture(struct texture *t)
{
    list_add_element(dsp.tex_list, t);
}

/**
 * Permet au créateur de carte de passer un pointer au module
 * d'affichage, donc le thread où l'OpenGL est exécuté peut 
 * en toute sécurité libérer la mémoire associée au modèle 
 * avant que le thread et le contexte de l'OpenGl termine.
 */
void display_register_model(struct model *m)
{
    list_add_element(dsp.mod_list, m);
}

