#ifndef PENGUINS_HEAP_H
#define PENGUINS_HEAP_H

/*
 * This heap structure cannot store anything else than void*s
 * of course it can be used for shorter types that fits inside the void*
 *
 */
struct heap;

struct heap *heap_create(size_t buffer_size,
			 void **buffer,
			 int (*compar__)(void*, void*),
			 int (*id__)(void *));
/* You can pass your own buffer to the heap structure and the
 * heap will use this buffer as the storage itself
 * It allows you to implement on-place heap sort rather easily
 * but THERE ARE SEVERAL RESTRICTION AND SOME THINGS THAT YOU NEED TO KNOW
 * the actual implementation uses array indices starting at 1.
 * Your buffer have to be one more than the actual buffer_size,
 * and you have to relocate the element at index 0, to the last index.
 *
 * You could also chose to sort only, the last buffer_size - 1 element in your
 * array and finally insert the first element yourself.
 *
 * I'm well aware that this is not convenient at all,
 * this may change for the better in future implementation.
 * consider using BSD's libc heapsort instead for the moment if this heap
 * doesn't prove to be useful.
 *
 ******
 *
 * the 'id' parameter is for updating the position
 * (i.e reajusting its position after some changes that would affect the way
 * compar evaluate it) of an element anywhere inside the heap, it must
 * return a different integer between 0 and buffer_size - 1,
 * for each void* element in your array.
 * To trigger the update of an element, call heap_update(h, element)
 */

void heap_destroy(struct heap *heap);
void *heap_extract_max(struct heap *heap);
void *heap_max(struct heap *heap);
void heap_insert(struct heap *heap, void *k);

void heap_update(struct heap *heap, void *k);
 // see the heap_create commentar above

size_t heap_size(struct heap *heap);

#endif // PENGUINS_HEAP_H
