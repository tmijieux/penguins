#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "utils/vec.h"
#include "utils/math.h"
#include "utils/list.h"
#include "utils/threading.h"

#include "penguins_opengl.h"

#include "display/display.h"
#include "display/display_internal.h"
#include "display/record.h"
#include "display/animator.h"


#include "d3v/d3v.h"
#include "d3v/object.h"
#include "d3v/shader.h"
#include "d3v/model.h"
#include "d3v/texture.h"
#include "d3v/mouse_projection.h"
#include "d3v/user_callback.h"

#include "server/map.h"
#include "server/server.h"


#define PENGUIN_FILE "models/wavefront/penguin.obj"
#define MAX_ADD_PENGUIN_JOB 50

typedef struct penguin_add_job {
    int tileid;
    int playerid;
} penguin_add_job_t;

int nb_pending_add_penguin_job = 0;
penguin_add_job_t penguin_add_jobs[MAX_ADD_PENGUIN_JOB];


/**
 * Gestion du thread.
 */
struct _display_thread {
    peng_thread_t t;
    peng_mutex_t m;
    peng_mutex_t penguin_add_mutex;
    int ThreadId;
};
static struct _display_thread display_thread;

struct display Display;

/**
 * Gestion du déroulement de la lecture de la partie.
 */
enum SENS {
    S_FORWARD,
    S_REWIND
};

/**
 * Lis et programme les animations à effectuer.
 * @param s - Sens de la lecture.
 */
static void display_schedule_animations(enum SENS s)
{
    int tileSrc = (s == S_FORWARD) ? record_get_current_src(Display.records)
        : record_get_rewind_src(Display.records);
    int tileDest = (s == S_FORWARD) ? record_get_current_dest(Display.records)
        : record_get_rewind_dest(Display.records);

    int setPenguin = (tileSrc == -1);
    if (setPenguin) {
        tileSrc = tileDest;
    }

    int penguin = Display.tile_penguins_id[tileSrc];

    if (!setPenguin) {
        setPenguin = (tileDest == -1 && s == S_REWIND);
    }
    if (setPenguin && penguin != -1) {
        if (anim_prepare()) {
            anim_new_movement(Display.penguin_objects[penguin], 0, 1);
            anim_set_hide(s != S_FORWARD);
            anim_push_movement();
            anim_launch();
        }
    } else {
        // Calculer le déplacement
        if (anim_prepare()) {
            if (penguin != -1) {
                vec3 tile_pos;
                vec3 pen_pos;

                d3v_object_get_position(Display.tile_objects[tileDest], &tile_pos);
                d3v_object_get_position(Display.penguin_objects[penguin], &pen_pos);

                float angle = angle_rotation_pingouin(&pen_pos, &tile_pos);
                int nb_move = NB_MOVE;
                if(angle <= 360) {
                    nb_move = NB_MOVE / 2;
                }

                anim_new_movement(Display.penguin_objects[penguin], 0, nb_move);
                if (angle > 360) {
                    anim_set_translation(tile_pos);
                }
                anim_set_rotation(angle);

                if (angle <= 360) {
                    anim_push_movement();
                    anim_new_movement(Display.penguin_objects[penguin], 0, NB_MOVE);
                    anim_set_translation(tile_pos);
                }
                anim_push_movement();
            }
            int tile = -1;
            if (s == S_FORWARD) {
                tile = tileSrc;
            } else {
                tile = tileDest;
            }
            anim_new_movement(Display.tile_objects[tile], 1, 1);
            anim_set_hide(s == S_FORWARD);
            anim_push_movement();
            anim_launch();
        }
    }

    if (tileSrc >= 0 && tileDest >= 0) {
        Display.tile_penguins_id[tileSrc] = -1;
        Display.tile_penguins_id[tileDest] = penguin;
    }
}

/**
 * Lance la lecture d'un mouvement dans l'historique.
 * @param s - Sens de la lecture.
 */
static void display_read_next_move(enum SENS s)
{
    int newMove = 0;
    if (s == S_FORWARD) {
        newMove = record_next(Display.records);
    }
    else if (s == S_REWIND) {
        newMove = record_previous(Display.records);
    }

    if (newMove) {
        d3v_request_animation_frame();
        display_schedule_animations(s);
    }
}

/*******************************************************/
/************ DRAWING METHODS **************************/

#ifdef DEBUG
/**
 * Dessine les lignes droites valides à partir d'une tuile pour
 * atteindre les autres tuiles.
 */
static void draw_direction_link(void)
{
    if (!Display.should_draw_links || Display.nb_links == 0) {
        return;
    }

    if (Display.active_tile_id >= Display.nb_tile_alloc) {
        fprintf(stderr, "draw_direction_link(): WARNING: looks like tile objects are not initialized!\n");
        return;
    }

    mat4 model;
    make_identity(&model);
    // model matrix is identity because links are already
    // positioned absolutely in world coordinates

    int program = Display.link_program;
    glUseProgram(program);
    int loc = glGetUniformLocation(program, "model");
    glUniformMatrix4fv(loc, 1, GL_TRUE, model.m);

    HANDLE_GL_ERROR(glBindVertexArray(Display.link_vao));
    HANDLE_GL_ERROR(glDrawArrays(GL_LINES, 0, 2*Display.nb_links));
}
#endif // DEBUG


object_t*
create_penguin(int id, model_t *m, texture_t *t, vec3 loc, float ROT, float SCALE)
{
    vec3 scale = { SCALE, SCALE, SCALE };
    vec3 rot = { 0., ROT, 0. };
    int program = get_or_load_shader(PENGUIN_SHADER_TEX_LIGHT);

    char name[40] = {0};
    snprintf(name, sizeof(name)-1, "penguin %d", id);
    return d3v_object_create(m, t, loc, rot, scale, program, name);
}

object_t*
create_tile(int id, model_t *m, texture_t *t, vec3 pos, float ROT, float SCALE)
{
    vec3 scale = { SCALE, SCALE, SCALE };
    vec3 rot = { 0., ROT, 0. };
    int program = get_or_load_shader(PENGUIN_SHADER_TEX_LIGHT);
    char name[40] = {0};
    snprintf(name, sizeof(name)-1, "tile %d", id);
    return d3v_object_create(m, t, pos, rot, scale, program, name);
}

/**
 * Permet au créateur de carte d'ajouter des pingouins à l'affichage.
 * @param tile - Tuile où se trouve le pingouin.
 * @param player - Identifiant du joueur.
 * @return int:  0 si l'ajout est valide.
 */
static int display_add_penguin_impl(int tileid, int playerid)
{
    int id = Display.nb_penguin_alloc;
    if (id < 0 || id >= Display.nb_penguin) {
        return -1;
    }
    int texid = playerid % Display.nb_penguin_tex;
    texture_t *tex = Display.penguin_textures[texid];

    vec3 pos;
    d3v_object_get_position(Display.tile_objects[tileid], &pos);

    Display.penguin_objects[id] = create_penguin(tileid, Display.penguin_model, tex, pos, 0.0, 0.1);
    if (Display.penguin_objects[id] == NULL) {
        return 0;
    }
    Display.tile_penguins_id[tileid] = id;
    // penguins are initially hidden:
    d3v_object_hide(Display.penguin_objects[id]);

    // add apparition of penguin to history record
    record_add(Display.records, -1, tileid);

    ++Display.nb_penguin_alloc;

    if (Display.autoplay) {
        d3v_request_animation_frame();
    }
    return 0;
}


static void handle_new_penguins()
{
    if (peng_mutex_trylock(&display_thread.penguin_add_mutex))
    {
        while (nb_pending_add_penguin_job > 0)
        {
            const penguin_add_job_t* job = &penguin_add_jobs[nb_pending_add_penguin_job-1];
            display_add_penguin_impl(job->tileid, job->playerid);
            --nb_pending_add_penguin_job;
        }

        peng_mutex_unlock(&display_thread.penguin_add_mutex);
    }
}

static void draw(void)
{
    handle_new_penguins();

    int nextMove = !anim_run();
    if (Display.autoplay && nextMove)
    {
        display_read_next_move(S_FORWARD);
    }

    #if DEBUG
    draw_direction_link();
    #endif // DEBUG

    for (int i = 0; i < Display.nb_penguin_alloc; ++i) {
        d3v_object_draw(Display.penguin_objects[i]);
    }

    for (int i = 0; i < Display.nb_tile_alloc; ++i) {

        object_t *obj = Display.tile_objects[i];
        d3v_object_draw(obj);

        int is_hidden = d3v_object_is_hidden(obj);

        int nb_fish = Display.tile_nb_fishes[i];
        int peng_id = Display.tile_penguins_id[i];

        if (is_hidden || nb_fish < 0 || nb_fish >=  10 || peng_id != -1) {
            // cannot render above 10
            continue;
        }
        char data[2] = {0};
        snprintf(data,2, "%d", nb_fish);

        vec3 pos;
        d3v_object_get_position(obj, &pos);
        pos.y += 0.05;

        d3v_draw_text(data[0], pos);
    }


}

static void prepare_link_data()
{
    Display.nb_links = 0;

    if (!Display.nb_tile_alloc || !Display.should_draw_links) {
        return;
    }
    Display.link_program = get_or_load_shader(PENGUIN_SHADER_SIMPLE_RED);

    vec3 src_pos;
    d3v_object_get_position(Display.tile_objects[Display.active_tile_id], &src_pos);
    for (int j = 0; j < Display.nb_tile; j++)
    {
        if (Display.link[Display.active_tile_id][j])
        {
            ++Display.nb_links;
        }
    }

    size_t num_elements = sizeof(float) * 6 * Display.nb_links;
    float *lines =  calloc(Display.nb_links, 6*sizeof(float));

    int k = 0;
    for (int j = 0; j < Display.nb_tile; j++)
    {
        if (Display.link[Display.active_tile_id][j])
        {
            vec3 dst_pos;
            d3v_object_get_position(Display.tile_objects[j], &dst_pos);

            lines[6*k+0] = src_pos.x;
            lines[6*k+1] = src_pos.y + 0.2; // elevation above the tiles
            lines[6*k+2] = src_pos.z;
            lines[6*k+3] = dst_pos.x;
            lines[6*k+4] = dst_pos.y + 0.2; // elevation above the tiles
            lines[6*k+5] = dst_pos.z;
            ++k;
        }
    }

    if (!Display.link_vao) {
        HANDLE_GL_ERROR(glGenVertexArrays(1, &Display.link_vao));
    }
    HANDLE_GL_ERROR(glBindVertexArray(Display.link_vao));
    HANDLE_GL_ERROR(glEnableVertexAttribArray(0));

    if (!Display.link_vbo) {
        HANDLE_GL_ERROR(glGenBuffers(1, &Display.link_vbo));
    }
    HANDLE_GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER, Display.link_vbo));
    HANDLE_GL_ERROR(glBufferData(GL_ARRAY_BUFFER, num_elements, lines, GL_STATIC_DRAW));
    HANDLE_GL_ERROR(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0));
    free(lines);

    /* HANDLE_GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER, 0)); */
    HANDLE_GL_ERROR(glBindVertexArray(0));
}


/***********************************************************/
/******* EVENTS MANAGEMENT **************************/

static void key_input(int key, int scancode, int action, int mods, int x, int y)
{
    switch (key) {
    case GLFW_KEY_P: // 'p'
        Display.autoplay = !Display.autoplay;
        if (Display.autoplay) {
            puts("autoplay on");
        } else {
            puts("autoplay off");
        }
        break;
    case GLFW_KEY_S: // 's' for surrender
        if (Display.mouseclick_mode)
        {
            vec3 pos = { -INFINITY };
            dsp_signal_game_thread(&pos);
        }
        break;
    case GLFW_KEY_UP: // up
        if (Display.active_tile_id < Display.nb_tile - 1)
        {
            ++Display.active_tile_id;
            prepare_link_data();
        }
        break;
    case GLFW_KEY_DOWN: // down
        if (Display.active_tile_id > 0)
        {
            --Display.active_tile_id;
            prepare_link_data();
        }
        break;
    case GLFW_KEY_LEFT: // left
        if (Display.autoplay) {
            puts("autoplay off");
            Display.autoplay = 0;
        }
        if (!anim_run())
        {
            display_read_next_move(S_REWIND);
        }
        break;
    case GLFW_KEY_RIGHT: // right
        if (!anim_run())
        {
            display_read_next_move(S_FORWARD);
        }
        break;
    }
}

static void mouse(int button, int state, int mods, int x, int y)
{
    switch (button) {
    case GLFW_MOUSE_BUTTON_1:
        if (state == GLFW_RELEASE && Display.mouseclick_mode)
        {
            vec3 pos;
            d3v_mouseproj(&pos, x, y);
            dsp_signal_game_thread(&pos);
        }
        break;
    }
}

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
 * @return int  -1 si l'ajout est invalide.
 */
static int display_add_tile(int tile_id, int model_id, int texture_id,
                            vec3 pos, int angle, float scale, int fish_count)
{
    // set camera at centroid of tiles
    static int weight = 0.;
    static vec3 _centroid = {0.,0.,0};
    _centroid.x = (weight * _centroid.x + pos.x) / (weight+1);
    _centroid.y = (weight * _centroid.y + pos.y) / (weight+1);
    _centroid.z = (weight * _centroid.z + pos.z) / (weight+1);
    weight ++;
    Display.centroid = _centroid;

    int id = Display.nb_tile_alloc;
    if (id < 0 || id >= Display.nb_tile) {
        return -1;
    }
    if (tile_id != id) {
        fprintf(stderr, "Incoherency during creation of tiles\n");
        exit(EXIT_FAILURE);
    }

    model_t *model = Display.tile_models[model_id];
    texture_t *tex = Display.tile_textures[texture_id];
    Display.tile_objects[id] = create_tile(tile_id, model, tex, pos, (float)angle, scale);
    Display.tile_nb_fishes[id] = fish_count;
    Display.tile_penguins_id[id] = -1;

    if (Display.tile_objects[id] == NULL) {
        printf("`display_add_tile()` failed\n");
        return -1;
    }
    ++Display.nb_tile_alloc;
    return 0;
}


// called from game thread
int display_add_penguin(int tileid, int playerid)
{
    if (!Display.thread_running) {
        fprintf(stderr, "Cannot add penguin objects if display thread is not running !!\n");
        return -1;
    }

    penguin_add_job_t job = {
        .playerid = playerid,
        .tileid = tileid
    };

    peng_mutex_lock(&display_thread.penguin_add_mutex);
    penguin_add_jobs[nb_pending_add_penguin_job++] = job;
    peng_mutex_unlock(&display_thread.penguin_add_mutex);
    return 0;
}

/***********************************************************/
/******* INITIALIZATION AND FREES **************************/

/*** INITS *****/

static void init_display_module(void)
{
    int nb_penguin = Display.nb_penguin;
    int nb_tile = Display.nb_tile;

    Display.should_draw_links = 0;
    Display.active_tile_id = 0;
    Display.link = calloc(nb_tile, sizeof(*Display.link));

    for (int i = 0; i < nb_tile; ++i) {
        Display.link[i] = calloc(nb_tile, sizeof(*Display.link[0]));
    }

    Display.autoplay = 1;
    Display.records = record_create(nb_tile + (nb_penguin * 2));

    Display.tile_objects = calloc(nb_tile, sizeof(Display.tile_objects[0]));
    Display.penguin_objects = calloc(nb_penguin, sizeof(Display.penguin_objects[0]));

    Display.tile_nb_fishes = calloc(nb_tile, sizeof(Display.tile_nb_fishes[0]));
    Display.tile_penguins_id = calloc(nb_tile, sizeof(Display.tile_penguins_id[0]));

    Display.nb_penguin_alloc = 0;
    Display.nb_tile_alloc = 0;
    Display.mouseclick_mode = 0;
}

static void load_models_and_textures(void)
{
    // penguins
    Display.penguin_model = model_load_wavefront(PENGUIN_FILE);
    Display.penguin_textures = malloc(sizeof(*Display.penguin_textures) * 4);
    Display.penguin_textures[0] = texture_load("textures/penguin_black.jpg");
    Display.penguin_textures[1] = texture_load("textures/penguin_red.jpg");
    Display.penguin_textures[2] = texture_load("textures/penguin_green.jpg");
    Display.penguin_textures[3] = texture_load("textures/penguin_blue.jpg");
    Display.nb_penguin_tex = 4;

    // tiles - registered from maps
    Display.tile_textures = calloc(
        Display.nb_registered_textures,
        sizeof(Display.tile_textures[0])
    );
    for (int i = 0; i < Display.nb_registered_textures; ++i) {
        Display.tile_textures[i] = texture_load(Display.textures[i]);
    }
    Display.tile_models = calloc(Display.nb_registered_models, sizeof(Display.tile_models[0]));
    for (int i = 0; i < Display.nb_registered_models; ++i) {
        Display.tile_models[i] = model_load_wavefront(Display.models[i]);
    }
}

static void load_tiles(void)
{
    for (int tile_id = 0; tile_id < Display.nb_tile; ++tile_id) {
        vdata_t data;
        graph_get_data(Game.tile_graph, tile_id, &data);
        display_add_tile(
            tile_id, data.model_id, data.texture_id,
            data.loc, data.angle, data.scale, data.nb_fish
        );
        // penguins cannot be added here because they might be added in parallel
    }
}

/*** EXITS *****/
/**
 * Libération de la mémoire utilisée avant de quitter.
 */
static void free_models_and_textures(void)
{
    // free penguins textures and models:
    for (int i = 0; i < Display.nb_penguin_tex; ++i) {
        texture_free(Display.penguin_textures[i]);
        Display.penguin_textures[i] = NULL;
    }
    free(Display.penguin_textures);
    Display.penguin_textures = NULL;

    // printf("free penguin model\n");
    model_free(Display.penguin_model);

    for (int i = 0; i < Display.nb_registered_textures; ++i) {
        texture_free(Display.tile_textures[i]);
    }
    for (int i = 0; i < Display.nb_registered_models; ++i) {
        model_free(Display.tile_models[i]);
    }
    free(Display.tile_textures);
    free(Display.tile_models);
}

/**
 *
 */
static void free_display_module(void)
{
    int count = Display.nb_penguin_alloc;
    Display.nb_penguin_alloc = 0;
    for (int i = 0; i < count; i++) {
        d3v_object_free(Display.penguin_objects[i]);
        Display.penguin_objects[i] = NULL;
    }
    free(Display.penguin_objects);
    Display.penguin_objects = NULL;

    count = Display.nb_tile_alloc;
    Display.nb_tile_alloc = 0;
    for (int i = 0; i < count; i++) {
        d3v_object_free(Display.tile_objects[i]);
        Display.tile_objects[i] = NULL;
    }
    free(Display.tile_objects);
    free(Display.tile_penguins_id);
    free(Display.tile_nb_fishes);
    Display.tile_objects = NULL;
    Display.tile_nb_fishes = NULL;
    Display.tile_penguins_id = NULL;

    record_free(Display.records);
    for (int i = 0; i < count; ++i) {
        free(Display.link[i]);
        Display.link[i] = NULL;
    }
    free(Display.link);
    Display.link = NULL;
}


/**
 * Permet au serveur d'ajouter des lignes droites à l'affichage.
 * @param src - Tuile d'origine de la ligne.
 * @param dst - Tuile de destination de la ligne.
 */
#ifdef DEBUG
static void display_add_link(int src, int dst)
{
    Display.should_draw_links = 1;
    Display.link[src][dst] = 1;
}


static void setup_directions_links(void)
{
    int nb_tile = Display.nb_tile;
    int nb_dimension = Display.nb_dimension;
    for (int i = 0; i < nb_tile; i++) {

        int nb_dir = map.get_number_directions(i, nb_dimension, nb_tile);
        for (int direction = 0; direction < nb_dir; ++direction) {
            int nb_moves = 1;
            int dest = move_is_valid_aux(i, direction, nb_moves);

            while (dest != -1) {
                display_add_link(i, dest);
                ++nb_moves;
                dest = move_is_valid_aux(i, direction, nb_moves);
            }
        }
    }
}
#endif


/*** STARTS *****/
static void* display_thread_main(void *args)
{
    peng_mutex_init(&display_thread.penguin_add_mutex);
    init_anim_module();
    init_display_module();

    // centroid not yet computed!!
    d3v_module_init(Display.nb_tile+Display.nb_penguin);

    load_models_and_textures();
    load_tiles();

    d3v_set_initial_camera_position(&Display.centroid);

    // TODO need to lock here and signal
    // the game thread if the game thread was waiting
    // for display thread initialization to finish
    Display.thread_running = 1;

    #ifdef DEBUG
    setup_directions_links();
    #endif
    prepare_link_data();

    d3v_set_draw_callback(&draw);
    d3v_set_key_input_callback(&key_input);
    d3v_set_mouse_callback(&mouse);

    d3v_main_loop();
    fprintf(stderr, "exiting display thread!\n");

    Display.thread_running = 0;
    Display.thread_terminated = 1;

    free_models_and_textures();
    free_display_module();

    if (Display.mouseclick_mode)
    {
        printf("leaving  main loop while forfeiting mouseclick\n");
        vec3 pos = {-INFINITY};
        dsp_signal_game_thread(&pos);
    }
    else
    {
        printf("not currently in  mouseclick\n");
    }

    peng_mutex_destroy(&display_thread.penguin_add_mutex);
    peng_thread_exit(DISPLAY_THREAD_RETVAL);
}


/**
 * Démarrer le thread d'affichage.
 */
void create_display_thread(int nb_dimension, int nb_tile, int nb_penguin)
{
    Display.nb_tile = nb_tile;
    Display.nb_penguin = nb_penguin;
    Display.nb_dimension = nb_dimension;

    #ifdef __unix__
    pthread_create(&display_thread.t, NULL, &display_thread_main, NULL);
    #else
    display_thread.t = CreateThread(
        NULL, 0,
        display_thread_main, NULL,
        0, &display_thread.ThreadId
    );
    #endif
}

int join_display_thread(void)
{
    void *retval = NULL;

    puts("Waiting for the user to end the display ...");
    #ifdef __unix__
    pthread_join(display_thread.t, &retval);
    #else
    WaitForSingleObject(display_thread.t, INFINITE);
    #endif
    fprintf(stderr, "exiting display thread!\n");

    if (retval == DISPLAY_THREAD_RETVAL) {
        puts("display thread exited successfully");
        return 0;
    }
    return -1;
}


/************************************************************/
/************ ADD ELEMENTS TO SCENE *************************/

/**
 * Permet au serveur d'ajouter des mouvement dans l'historique de la
 * partie.
 * @param src - Tuile d'origine du mouvement.
 * @param dst - Tuile de destination du mouvement.
 * @return int - 1 si l'enregistrement est valide.
 */
int display_add_move(int src, int dst)
{
    if (!Display.thread_running) {
        return -1;
    }
    int id = record_add(Display.records, src, dst);

    if (Display.autoplay) {
        d3v_request_animation_frame();
    }
    return id;
}

int display_register_texture(const char *t)
{
    int id = Display.nb_registered_textures++;
    if (id >= DISPLAY_TEXTURE_MAX) {
        return -1;
    }
    Display.textures[id] = t;
    return id;
}

int display_register_model(const char *modelpath)
{
    int id = Display.nb_registered_models++;
    if (id >= DISPLAY_MODEL_MAX) {
        return -1;
    }
    Display.models[id] = modelpath;
    return id;
}

