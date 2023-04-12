#include <stdlib.h>
#include <utils/list_node.h>

/**
 * @file list_node.c
 */

/**
 * Description d'un noeud.
 */
struct node {
    void *data;
    int is_sentinel;
    struct node *next;
    struct node *previous;
};

/**
 * Création d'un noeud.
 * @param data - L'élément à stocker.
 * @param flags - Options.
 * 1 -> Noeud sentinel.
 * 2 -> Noeud contenant des données.
 * @return struct node * - Le noeud créé.
 */
struct node *node_create(void *data, int flags)
{
    struct node *node = malloc(sizeof(*node));
    if ((flags & NODE_DATA) != 0)
	node->data = data;
    node->is_sentinel = ((flags & NODE_SENTINEL) != 0);
    return node;
}

/**
 * Libérer la mémoire occupée par un noeud.
 * @param node - Noeud à supprimer.
 */
void node_free(struct node *node)
{
    free(node);
}

/**
 * Obtenir le noeud suivant.
 * @param node - Noeud actuel.
 * @return struct node * - Noeud suivant.
 */
struct node *node_get_next(struct node *node)
{
    return node->next;
}

/**
 * Obtenir le noeud précédent.
 * @param node - Noeud actuel.
 * @return struct node * - Noeud précédent.
 */
struct node *node_get_previous(struct node *node)
{
    return node->previous;
}

/**
 * Changer le noeud suivant.
 * @param node - Noeud à modifier.
 * @param next - Nouveau noeud suivant.
 */
void node_set_next(struct node *node, struct node *next)
{
    node->next = next;
}

/**
 * Changer le noeud précédent.
 * @param node - Noeud à modifier.
 * @param previous - Nouveau noeud  précédent.
 */
void node_set_previous(struct node *node, struct node *previous)
{
    node->previous = previous;
}

/**
 * Obtenir l'élément contenu dans un noeud.
 * @param node - Noeud à lire.
 * @return void * - L'élément contenu par le noeud.
 */
void *node_get_data(struct node *node)
{
    return node->data;
}

/**
 * Changer l'élément contenu dans un noeud.
 * @param node - Noeud à modifier.
 * @param data - L'élément à contenir.
 */
void node_set_data(struct node *node, void *data)
{
    node->data = data;
}

/**
 * Savoir si le noeud est sentinelle.
 * @param node - Noeud à lire.
 * @return 0 si non sentinelle, autre valeur sinon
 */
int node_is_sentinel(struct node *node)
{
    return node->is_sentinel;
}
