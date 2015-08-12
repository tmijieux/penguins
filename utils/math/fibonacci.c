#include <stdio.h>
#include <stdlib.h>

#include <utils/fibonacci.h>

/**
 * Description de fibonacci.
 */
struct fibonacci{
    int *buffer;
    int buffer_size;
    int nb_int;
};

/**
 * Initialisation de fibonacci.
 * @return struct fibonacci * - Fibo
 */
struct fibonacci *fibonacci_init(void)
{
    struct fibonacci *f = malloc(sizeof(struct fibonacci));
    f->buffer_size = 4;
    f->nb_int = 2;
    f->buffer = malloc(f->buffer_size * sizeof(int));
    f->buffer[0] = 0;
    f->buffer[1] = 1;
    return f;
}

/**
 * Obtenir le n-ième élément de la suite de fibonacci.
 * @param f - Structure fibonacci stockant les calculs.
 * @param n - Position de l'élément.
 * @return int - Valeur du n-ième élément.
 */
int fibonacci_get(struct fibonacci *f, int n)
{
    if (n >= f->nb_int) {
	if (n >= f->buffer_size) {
	    f->buffer_size = 2 * n;
	    f->buffer = realloc(f->buffer, 
				f->buffer_size * 
				sizeof(int));
	}

	for (int i = f->nb_int; i <= n; i++)
	    f->buffer[i] = f->buffer[i - 1] + f->buffer[i - 2];
	f->nb_int = n + 1;
    }
    return f->buffer[n];
}

/**
 * Libération de la mémoire occupée par fibo.
 * @param f - Structure fibonacci.
 */
void fibonacci_free(struct fibonacci *f)
{
    free(f->buffer);
    free(f);
}
