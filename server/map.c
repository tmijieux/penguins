#include <stdio.h>
#include <server/map.h>
#include <utils/graph.h>
#include <dlfcn.h>

/**
 * @file map.c
 */

/**
 * Initialisation du graphe.
 * @param int - Dimension du jeu : 1D, 2D, 3D, ...
 * @param int - Nombre de tuiles.
 * @param struct graph * - Graphe du jeu.
 */
void (*map_init_graph) (int, int, struct graph *);

/**
 * Obtenir la destination d'un mouvement.
 * @param int - Origine du mouvement (identifiant de la tuile).
 * @param int * - Direction du mouvement.Le mappeur peut se permettre 
 * de changer sa valeur pour indiquer au serveur quelle est la prochaine 
 * direction à prendre pour avoir  un déplacement en ligne droite.
 * @param int - Dimension du jeu.
 * @param int - Nombre total de tuiles.
 * @param struct graph * - Graphe du jeu.
 * @return int - Destination du mouvment
 */
int (*map_get_id_from_move) (int, int*, int, int, struct graph*);

/**
 * Obtenir le nombre de directions d'une tuile.
 * @param int - Identifiant de la tuile.
 * @param int - Dimension du jeu.
 * @param int - Nombre total de tuiles.
 * @return int - Nombre de directions.
 */
int (*map_get_number_directions) (int, int, int);

static void *handle;

/**
 * Charger la fonction souhaitée.
 * @param funpointer_addr - Pointeur de fonction (destinataire).
 * @param symbol - Nom de la fonction à charger.
 */
static void load_function_(void **funpointer_addr, const char *symbol)
{
    *funpointer_addr = *(void**)dlsym(handle, symbol);
    if (*funpointer_addr == NULL) {
	puts(dlerror());
	exit(EXIT_FAILURE);
    }
}

#define load_function(x) load_function_(((void**)(&x)), #x)

/**
 * Chargement du module map.
 * @param map_path - Chemin d'accès au module.
 */
void map_init(const char *map_path)
{
    if ((handle = dlopen(map_path, RTLD_NOW)) == NULL) {
	puts(dlerror());
	exit(EXIT_FAILURE);
    }

    load_function(map_init_graph);
    load_function(map_get_id_from_move);
    load_function(map_get_number_directions);
}

/**
 * Déchargement du module map.
 */
void map_exit(void)
{
    dlclose(handle);
}
