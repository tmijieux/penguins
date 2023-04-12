#include <stdlib.h>
#include <utils/list_node.h>
#include <utils/sorted_list.h>

/**
 * @file sorted_list.c
 */

/**
 * Description d'un liste triée.
 */
struct sorted_list {
    int (*compare)(void*, void*);
    int flags;
    struct node *front_sentinel;
    size_t size;
    unsigned int cursor_pos;
    struct node *cursor;
};

/**
 * Créer une liste triée.
 * @param flags - Option de la liste.
 * 0 -> default.
 * 1 -> Libérer les éléments alloués.
 * @param compare - Fonction de comparaison de deux éléments.
 * @return struct sorted_list * - La liste triée créée.
 */
struct sorted_list *slist_create(int flags,
				 int (*compare)(void*, void*))
{
    struct sorted_list *sl = malloc(sizeof(*sl));
    sl->compare = compare;
    sl->size = 0;
    sl->flags = flags;
    sl->front_sentinel = node_create(NULL, NODE_SENTINEL);
    struct node *back_sent = node_create(NULL, NODE_SENTINEL);
    node_set_next(sl->front_sentinel, back_sent);
    sl->cursor = sl->front_sentinel;
    sl->cursor_pos = 0;
    return sl;
}

/**
 * Obtenir le n-ième noeud de la liste.
 * @param sl - Liste triée à parcourir.
 * @param pos - Position du noeud.
 * @return struct node * - Le n-ième noeud. 
 */
struct node *slist_get_node(struct sorted_list *sl,
			    unsigned int pos)
{
    int k = pos;
    struct node *it = sl->front_sentinel;
    if (sl->cursor_pos <= k) {
	k -= sl->cursor_pos;
	it = sl->cursor;
    }
    for (int i = 0; i < k; i++)
	it = node_get_next(it);
    sl->cursor = it;
    sl->cursor_pos = pos;
    return it;
}

/**
 * Obtenir l'élément à la n-ième position. 
 * @param sl - La liste triée à parcourir.
 * @param pos - Position de l'élément.
 * @return void * - L'élément. 
 */
void *slist_get_data(struct sorted_list *sl, unsigned int pos)
{
    return node_get_data(slist_get_node(sl, pos));
}

/**
 * Obtenir la taille d'une liste triée.
 * @param lsl - La liste triée.
 * @return size_t - Taille de la liste.
 */
size_t slist_size(struct sorted_list *sl)
{
    return sl->size;
}

/**
 * Supprimer le n-ième élément.
 * @param sl - La liste triée à modifier.
 * @param pos - Position de l'élément à supprimer.
 */
void slist_remove(struct sorted_list *sl, int pos)
{
    struct node *prev = slist_get_node(sl, pos-1);
    struct node *rm = node_get_next(prev);
    node_set_next(prev, node_get_next(rm));
    node_free(rm);
    sl->size --;
}

/**
 * Obtenir la position où devrait se trouver l'élément à rechercher.
 * @param sl - La liste triée à parcourir.
 * @param value - L'élément à rechercher.
 * @return unsigned int - Position où devrait se trouver l'élément. 
 */
static unsigned int slist_find_value_index(struct sorted_list *sl, void *value)
{
    size_t n = slist_size(sl);
    for (int i = 0; i < n; i++) {
	struct node *node = slist_get_node(sl, i);
	if (!(sl->compare(value, node_get_data(node_get_next(node))) > 0))
	    return i+1;
    }
    return 0;
}

/**
 * Rechercher si un élément est présent.
 * @param sl - La liste triée à parcourir.
 * @param value - L'élément à rechercher.
 * @return int - 1 si l'élément est présent, 0 sinon. 
 */
int slist_find_value(struct sorted_list *sl, void *value)
{
    int idx = slist_find_value_index(sl, value);
    if (idx != 0)
	return sl->compare(slist_get_data(sl, idx), value) == 0;
    return 0;
}

/**
 * Ajouter un élément dans la liste triée.
 * @param sl - La liste triée à modifier.
 * @param value - L'élément à ajouter.
 */
void slist_add_value(struct sorted_list *sl, void *value)
{
    int idx = slist_find_value_index(sl, value);
    struct node *prev;
    if (idx > 0)
	prev = slist_get_node(sl, idx-1);
    else
	prev = slist_get_node(sl, slist_size(sl));
    struct node *add = node_create(value, NODE_DATA);
    node_set_next(add, node_get_next(prev));
    node_set_next(prev, add);
    sl->size ++;
}

/**
 * Supprimer un élément de la liste triée.
 * @param sl - La liste triée à modifier.
 * @param value - L'élément à supprimer.
 */
void slist_remove_value(struct sorted_list *sl, void *value)
{
    int idx = slist_find_value_index(sl, value);
    if (idx != 0)
	slist_remove(sl, idx);
}

/**
 * Libérer la mémoire occupée par la liste triée.
 * @param sl - La liste triée à supprimer.
 */
void slist_destroy(struct sorted_list *sl)
{
    while (slist_size(sl) > 0) {
	slist_remove(sl, 1);
    }
    node_free(node_get_next(sl->front_sentinel));
    node_free(sl->front_sentinel);
    free(sl);
}
