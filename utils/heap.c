#include <stdlib.h>
#include <utils/heap.h>

/**
 * @file heap.c
 */

/**
 * Description d'un tas.
 */
struct heap {
    int (*compar)(void*, void*);
    int (*id)(void*);
    size_t heap_size;
    void **buf;
    int *position;
    int free_buf;
};

/**
 * Créer un tas.
 * @param buffer_size - Taille du tas.
 * @param buffer - Buffer externe.
 * @param compar__ - Fonction de comparaison de deux éléments.
 * @param id__ - Fonction permettant de récupérer l'identifiant d'un
 * élément.
 * @return struct heap * - Le tas créé.
 */
struct heap *heap_create(size_t buffer_size,
			 void **buffer,
			 int (*compar__)(void*, void*),
			 int (*id__)(void *))
{
    struct heap *heap = malloc(sizeof(*heap));
    heap->compar = compar__;
    heap->id = id__;
    heap->heap_size = 0;
    if (buffer == NULL) {
	heap->buf = malloc(sizeof(*(heap->buf)) * (buffer_size + 1));
	heap->free_buf = 1;
    } else {
	heap->buf = buffer;
	heap->free_buf = 0;
    }
    if (id__ != NULL)
	heap->position = malloc(buffer_size * sizeof(*heap->position));
    else
	heap->position = NULL;
    
    return heap;
}

/**
 * Libérer la mémoire occupée par un tas.
 * @param heap - Le tas à supprimer.
 */
void heap_destroy(struct heap *heap)
{
    if (heap->free_buf)
	free(heap->buf);
    free(heap->position);
    free(heap);
}

/**
 *
 * 
 */
static inline int heap_greater_child(struct heap *heap, int i)
{
    if (2*i == heap->heap_size)
	return 2*i;
    if (heap->compar(heap->buf[2*i], heap->buf[2*i+1]) > 0)
	return 2*i;
    return 2*i+1;
}

/**
 *
 * 
 */
static inline void heap_swap(struct heap *heap, int i, int k)
{
    if (heap->id != NULL) {
	int id_i = heap->id(heap->buf[i]);
	int id_k = heap->id(heap->buf[k]);
	heap->position[id_i] = k;
	heap->position[id_k] = i;
    }
    void *tmp = heap->buf[i];
    heap->buf[i] = heap->buf[k];
    heap->buf[k] = tmp;
}

/**
 *
 * 
 */
static inline int heap_extract_problem(struct heap *heap, int i)
{
    return ((2*i == heap->heap_size &&
	     heap->compar(heap->buf[2*i], heap->buf[i]) > 0) ||
	    (2*i+1 <= heap->heap_size &&
	     (heap->compar(heap->buf[2*i], heap->buf[i]) > 0 ||
	      heap->compar(heap->buf[2*i+1], heap->buf[i]) > 0)));
}

/**
 *
 * 
 */
static int heap_extract_resolve(struct heap *heap, int i)
{
    int k = heap_greater_child(heap, i);
    heap_swap(heap, i, k);
    return k;
}

/**
 *
 * 
 */
void *heap_extract_max(struct heap *heap)
{
    void *max = heap->buf[1];
    heap->buf[1] = heap->buf[heap->heap_size--];
    int i = 1;
    while (heap_extract_problem(heap, i)) {
	i = heap_extract_resolve(heap, i);
    }
    return max;
}

/**
 *
 * 
 */
void *heap_max(struct heap *heap)
{
    return heap->buf[1];
}

/**
 *
 * 
 */
static inline int heap_insert_problem(struct heap *heap, int i)
{
    if (i == 1) return 0;
    return heap->compar(heap->buf[i], heap->buf[i/2]) > 0;
}

/**
 *
 * 
 */
static inline int heap_insert_resolve(struct heap *heap, int i)
{
    heap_swap(heap, i, i/2);
    return i/2;
}

/**
 *
 * 
 */
void heap_insert(struct heap *heap, void *k)
{
    int i = ++heap->heap_size;
    heap->buf[i] = k;
    if (heap->id != NULL) {
	int id = heap->id(k);
	heap->position[id] = i;
    }
    
    while (heap_insert_problem(heap, i)) {
	i = heap_insert_resolve(heap, i);
    }
}

/**
 *
 * 
 */
void heap_update(struct heap *heap, void *k)
{
    if (heap->id == NULL)
	return;
    int id = heap->id(k);
    int i = heap->position[id];
    
    while (heap_insert_problem(heap, i)) {
	i = heap_insert_resolve(heap, i);
    }
    while (heap_extract_problem(heap, i)) {
	i = heap_extract_resolve(heap, i);
    } // WARNING: may not work
}

/**
 * Obtenir la taille d'un tas.
 * @param heap - Le tas à analiser.
 * @return size_t - La taille de tas.
 */
size_t heap_size(struct heap *heap)
{
    return heap->heap_size;	
}
