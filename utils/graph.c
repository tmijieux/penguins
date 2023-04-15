#include <stdlib.h>

#include "utils/vec.h"
#include "utils/sorted_list.h"
#include "utils/graph.h"


/**
 * @file graph.c
 */

/**
 * Description d'une liste des successeurs avec itérateur.
 */
struct successor_iterator {
    int idx;
    sorted_list_t *successors;
};

/**
 * Description d'un sommet du graphe.
 */


struct vertex {
    int *neighbours;
    struct vert_data data;
};
typedef struct vertex vertex_t;



/**
 * Description d'un graphe.
 */
struct graph {
    int is_oriented;
    int vert_count;
    vertex_t *vert;
    sorted_list_t *edges[];
};


/******************** iterator ***********************************/
/**
 * Placer l'itérateur au début pour une liste des successeurs.
 * @param sc - Liste des successeurs.
 */
void iterator_begin(successor_iter_t *sc)
{
    sc->idx = 1;
    //list_get_node(sc->successor, 1);
}

/**
 * Obtenir la liste des successeurs d'un sommet.
 * @return successor_iter_t * - Liste des successeurs.
 * @param g - Le graphe.
 * @param vertex - Identifiant d'un sommet dans le graphe.
 */
successor_iter_t
*graph_get_successor_iterator(graph_t *g, int vertex)
{
    successor_iter_t *sc = malloc(sizeof(*sc));
    sc->successors = g->edges[vertex];
    iterator_begin(sc);
    return sc;
}

/**
 * Libérer la mémoire occupée par une liste des successeurs.
 * @param sc - Liste des successeurs.
 */
void successor_iterator_free(successor_iter_t *sc)
{
    free(sc);
}

/**
 * Avancer l'itérateur d'une liste des successeurs.
 * @param sc - Liste des successeurs.
 */
void iterator_next(successor_iter_t *sc)
{
    sc->idx ++;
}

/**
 * Savoir si l'itérateur de la liste des successeurs est
 * à la fin.
 * @param sc - Liste des successeurs.
 * @return int - 1 si c'est la fin, 0 sinon.
 */
int iterator_end(successor_iter_t *sc)
{
    return sc->idx == slist_size(sc->successors) + 1;
}

/**
 * Obtenir le successeur à la position de l'itérateur d'une
 * liste des successeurs.
 * @param sc - Liste des successeurs.
 * @return int - Identifiant du successeur.
 */
int iterator_value(successor_iter_t *sc)
{
    return (int)(long)slist_get_data(sc->successors, sc->idx);
}

/***************************************************************/

/**
 * Comparaison de deux entiers.
 * @param a - Premier entier.
 * @param b - Deuxième entier.
 * @return int - 0 si égaux, autre sinon.
 */
static int compare_int(void *a_, void *b_)
{
    return (int)(unsigned long) a_ - (int)(unsigned long) b_;
}

/**
 * Création d'un graphe.
 * @param vert_count - Nombre de sommets.
 * @param is_oriented - 1 Si le graphe est orienté, 0 sinon.
 * @param imprementation - Non utilisé dans cette version.
 * @return graph_t * - Le graphe.
 */
graph_t *graph_create(int vert_count, int is_oriented, int implementation)
{
    graph_t *gr = calloc(vert_count, sizeof(*gr) + sizeof(gr->edges[0]));
    gr->is_oriented = is_oriented;
    gr->vert_count = vert_count;
    for (int i = 0; i < vert_count; i++) {
	gr->edges[i] = slist_create(0, &compare_int);
    }
    gr->vert = calloc(vert_count, sizeof(gr->vert[0]));
    for (int i = 0; i < vert_count; i++) {
	gr->vert[i].data.player_id = -1;
    }
    return gr;
}

/**
 * Obtenir la taille du graphe (nombre sommets).
 * @param G - Le graphe.
 * @return size_t - Nombre de sommets dans le graphe.
 */
size_t graph_size(const graph_t *G)
{
    return G->vert_count;
}

/**
 * Obtenir le nombre d'arêtes sortantes d'un sommet.
 * @param g - Le graphe.
 * @param vertex - Identifiant du sommet.
 * @return int - Nombre d'arêtes sortantes du sommet.
 */
int graph_edge_count(const graph_t *g, int vertex)
{
    return slist_size(g->edges[vertex]);
}

/**
 * Savoir si le graphe est orienté.
 * @param G - Le graphe.
 * @return int - 1 si orienté, 0 sinon.
 */
int graph_is_oriented(const graph_t *G)
{
    return G->is_oriented;
}

/**
 * Libérer la mémoire occupée par le graphe.
 * @param gr - Le graphe.
 */
void graph_destroy(graph_t *gr)
{
    for (int i = 0; i < gr->vert_count; i++)
	slist_destroy(gr->edges[i]);
    free(gr->vert);
    free(gr);
}

/**
 * Savoir si une arête existe entre deux sommets.
 * @param gr - Le graphe.
 * @param v_src - Sommet source.
 * @param v_dst - Sommet de destination.
 * @return int - 1 si existe, 0 sinon.
 */
int graph_has_edge(const graph_t *gr, int v_src, int v_dst)
{
    return slist_find_value(gr->edges[v_src], (void*)(long)v_dst);
}

/**
 * Ajouter une arête entre deux sommets.
 * @param gr - Le graphe.
 * @param v_src - Sommet source.
 * @param v_dst - Sommet de destination.
 */
void graph_add_edge(graph_t *gr , int v_src, int v_dst)
{
    if (!graph_has_edge(gr, v_src, v_dst))
	slist_add_value(gr->edges[v_src], (void*)(long)v_dst);
    if (!gr->is_oriented)
	if (!graph_has_edge(gr, v_dst, v_src))
	    slist_add_value(gr->edges[v_dst], (void*)(long)v_src);
}

/**
 * Supprimer une arête entre deux sommets.
 * @param gr - Le graphe.
 * @param v_src - Sommet source.
 * @param v_dst - Sommet de destination.
 */
void graph_remove_edge(graph_t *gr , int v_src, int v_dst)
{
    slist_remove_value(gr->edges[v_src], (void*)(long)v_dst);
    if (!gr->is_oriented)
	slist_remove_value(gr->edges[v_dst], (void*)(long)v_src);
}

/**
 * Copier un graphe.
 * @param G - Graphe à copier.
 * @return graph_t * - Copie du graphe.
 */
graph_t *graph_copy(const graph_t *G)
{
    graph_t *H = graph_create(G->vert_count, G->is_oriented,
				   GRAPH_LIST);
    for (int i = 0; i < G->vert_count; i++) {
	size_t s = slist_size(G->edges[i]);
	for (int j = 1; j <= s; j++) {
	    void *succ = slist_get_data(G->edges[i], j);
	    slist_add_value(H->edges[i], succ);
	}
	H->vert[i] = G->vert[i];
    }
    return H;
}

/**
 * Tranposer un graphe (si il est orienté).
 * @param G - Graphe à transposé (modifié).
 */
void graph_transpose(graph_t *G)
{
    if (!graph_is_oriented(G))
	return;
    sorted_list_t **edges = malloc(sizeof(*edges) * G->vert_count);
    for (int i = 0; i < G->vert_count; i++) {
	edges[i] = slist_create(0, &compare_int);
    }
    for (int i = 0; i < G->vert_count; i++) {
	sorted_list_t *sl = G->edges[i];
	while (slist_size(sl) > 0) {
	    int j = (int)(long) slist_get_data(sl, 1);
	    slist_remove(sl, 1);
	    slist_add_value(edges[j], (void*)(long)i);
	}
    }
    for (int i = 0; i < G->vert_count; i++) {
	slist_destroy(G->edges[i]);
	G->edges[i] = edges[i];
    }
}

/**
 * Changer les données sur un sommet du graphe.
 * @param g - Le graphe.
 * @param vertex - Identifiant du sommet à modifier.
 * @param fish - Nombre de poissons.
 */
void graph_set_data(graph_t *g, int vertex, const vdata_t *data)
{
    g->vert[vertex].data = *data;
}

/**
 * Obtenir les données sur un sommet du graphe.
 * @param g - Le graphe.
 * @param vertex - Identifiant du sommet à modifier.
 * @return int - Nombre de poissons.
 */
void graph_get_data(graph_t *g, int vertex, vdata_t *out)
{
    *out = g->vert[vertex].data;
}

void graph_set_nb_fish(graph_t *g, int vertex, int nb_fish)
{
    g->vert[vertex].data.nb_fish = nb_fish;
}
int graph_get_nb_fish(graph_t *g, int vertex)
{
    return g->vert[vertex].data.nb_fish;
}

void graph_set_player_id(graph_t *g, int vertex, int player_id)
{
    g->vert[vertex].data.player_id = player_id;
}
int graph_get_player_id(graph_t *g, int vertex)
{
    return g->vert[vertex].data.player_id;
}



/**
 * Changer les voisins d'un sommet du graphe.
 * @param g - Le graphe.
 * @param vertex - Identifiant du sommet à modifier.
 * @param neighbours - Tableau contenant les identifiants des voisins.
 */
void graph_set_neighbours(graph_t *g, int vertex, int *neighbours)
{
    g->vert[vertex].neighbours = neighbours;
}

/**
 * Obtenir les voisins d'un sommet du graphe.
 * @param g - Le graphe.
 * @param vertex - Identifiant du sommet à modifier.
 * @retunr int * - Tableau contenant les identifiants des voisins.
 */
int *graph_get_neighbours(graph_t *g, int vertex)
{
    return g->vert[vertex].neighbours;
}
