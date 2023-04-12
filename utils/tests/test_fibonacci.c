#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include <utils/fibonacci.h>

/**
 * @file
 */

void test_get(void)
{
    struct fibonacci * f = fibonacci_init();
    
    assert(fibonacci_get(f, 3) == 2);
    assert(fibonacci_get(f, 4) == 3);
    assert(fibonacci_get(f, 8) == 21);
    assert(fibonacci_get(f, 30) == 832040);
    fibonacci_free(f);
}

int main(int argc, char **argv)
{
    test_get();
    return EXIT_SUCCESS;
}
