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
    node_t *next;
    node_t *previous;
};

/**
 * Création d'un noeud.
 * @param data - L'élément à stocker.
 * @param flags - Options.
 * 1 -> Noeud sentinel.
 * 2 -> Noeud contenant des données.
 * @return node_t * - Le noeud créé.
 */
node_t *node_create(void *data, int flags)
{
    node_t *node = malloc(sizeof(*node));
    if ((flags & NODE_DATA) != 0) {
	node->data = data;
    }
    node->is_sentinel = ((flags & NODE_SENTINEL) != 0);
    return node;
}

/**
 * Libérer la mémoire occupée par un noeud.
 * @param node - Noeud à supprimer.
 */
void node_free(node_t *node)
{
    free(node);
}

/**
 * Obtenir le noeud suivant.
 * @param node - Noeud actuel.
 * @return node_t * - Noeud suivant.
 */
node_t *node_get_next(node_t *node)
{
    return node->next;
}

/**
 * Obtenir le noeud précédent.
 * @param node - Noeud actuel.
 * @return node_t * - Noeud précédent.
 */
node_t *node_get_previous(node_t *node)
{
    return node->previous;
}

/**
 * Changer le noeud suivant.
 * @param node - Noeud à modifier.
 * @param next - Nouveau noeud suivant.
 */
void node_set_next(node_t *node, node_t *next)
{
    node->next = next;
}

/**
 * Changer le noeud précédent.
 * @param node - Noeud à modifier.
 * @param previous - Nouveau noeud  précédent.
 */
void node_set_previous(node_t *node, node_t *previous)
{
    node->previous = previous;
}

/**
 * Obtenir l'élément contenu dans un noeud.
 * @param node - Noeud à lire.
 * @return void * - L'élément contenu par le noeud.
 */
void *node_get_data(node_t *node)
{
    return node->data;
}

/**
 * Changer l'élément contenu dans un noeud.
 * @param node - Noeud à modifier.
 * @param data - L'élément à contenir.
 */
void node_set_data(node_t *node, void *data)
{
    node->data = data;
}

/**
 * Savoir si le noeud est sentinelle.
 * @param node - Noeud à lire.
 * @return 0 si non sentinelle, autre valeur sinon
 */
int node_is_sentinel(node_t *node)
{
    return node->is_sentinel;
}
