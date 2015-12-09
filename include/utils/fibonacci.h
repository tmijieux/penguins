#ifndef FIBONACCI_H
#define FIBONACCI_H

struct fibonacci;

// Returns a pointer to initialized fibonacci structure
struct fibonacci *fibonacci_init();
// Compute the nth value of the fibonacci sequence
int fibonacci_get(struct fibonacci *f, int n);
// Free the fibonacci structure
void fibonacci_free(struct fibonacci *f);

#endif //FIBONACCI_H
