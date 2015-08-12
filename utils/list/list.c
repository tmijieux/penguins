#include <stdlib.h>

#include <utils/list.h>
#include <utils/list_node.h>

/**
 * @file list.c
 */

/**
 * Description d'un liste.
 */
struct list {
    struct node *front_sentinel;
    struct node *cursor;
    size_t size;
    unsigned int pos_cursor;
    unsigned int flags;
};

/**
 * Obtenir le n-ième noeud de la liste.
 * @param list - Liste à parcourir.
 * @param n - Position du noeud.
 * @return static struct node * - Le n-ième noeud. 
 */
static struct node *list_get_node(struct list *list, unsigned int n)
{
    int k = n;
    struct node *node = list->front_sentinel;
    if (list->pos_cursor <= k) {
	k -= list->pos_cursor;
	node = list->cursor;
    }
    for (int i = 0; i < k; i++)
	node = node_get_next(node);
    list->cursor = node;
    list->pos_cursor = n;
    return node;
}

/**
 * Obtenir la taille d'une liste.
 * @param list - La liste.
 * @return size_t - Taille de la liste.
 */
size_t list_size(struct list *list)
{
    return list->size;
}

/**
 * Créer une liste.
 * @param flags - Option de la liste.
 * 0 -> default.
 * 1 -> Libérer les éléments alloués.
 * @return struct list * - La liste créée.
 */
struct list *list_create(int flags)
{
    struct list *list = malloc(sizeof(*list));
    list->front_sentinel = node_create(NULL, NODE_SENTINEL);
    node_set_next(list->front_sentinel, node_create(NULL, NODE_SENTINEL));
    list->flags = flags;
    list->cursor = list->front_sentinel;
    list->pos_cursor = 0;
    list->size = 0;
    return list;
}

/**
 * Libérer la mémoire occupée par une liste.
 * @param list - Liste à supprimer.
 */
void list_destroy(struct list *list)
{
    while (list_size(list) > 0)
	list_remove_element(list, 1);
    node_free(node_get_next(list->front_sentinel));
    node_free(list->front_sentinel);
    free(list);
}

/**
 * Obtenir l'élément à la n-ième position. 
 * @param list - La liste à parcourir.
 * @param n - Position de l'élément.
 * @return void * - L'élément. 
 */
void *list_get_element(struct list *list, unsigned int n)
{
    return node_get_data(list_get_node(list, n));
}

/**
 * Ajouter un élément en début de liste.
 * @param list - La liste à modifier.
 * @param element - L'élément à ajouter.
 */
void list_add_element(struct list *list, void *element)
{
    struct node *tmp = node_create(element, NODE_DATA);
    node_set_next(tmp, node_get_next(list->front_sentinel));
    node_set_next(list->front_sentinel, tmp);
    list->size ++;
}

/**
 * Insérer un élément à la n-ième position.
 * @param list - La liste à modifier.
 * @param n - Position de l'élément à insérer.
 * @param element - L'élément à insérer.
 */
void list_insert_element(struct list *list, unsigned int n, void *element)
{
    struct node *previous = list_get_node(list, n-1);
    struct node *tmp = node_create(element, NODE_DATA);
    node_set_next(tmp, node_get_next(previous));
    node_set_next(previous, tmp);
    list->size ++;
}

/**
 * Supprimer le n-ième élément.
 * @param list - La liste à modifier.
 * @param n - Position de l'élément à supprimer.
 */
void list_remove_element(struct list *list, unsigned int n)
{
    struct node *previous = list_get_node(list, n-1);
    struct node *tmp = node_get_next(previous);
    node_set_next(previous, node_get_next(tmp));
    if ((list->flags & LIST_FREE_MALLOCD_ELEMENT__) != 0)
	free(node_get_data(tmp));
    node_free(tmp);
    list->size --;
}
