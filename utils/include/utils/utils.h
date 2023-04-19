#ifndef PENGUINS_UTIL_H
#define PENGUINS_UTIL_H

int is_shared_library(const char *filename);

#ifndef _WIN32

# define min(a,b)                               \
    ({ __typeof__ ((a)) _a = (a);               \
        __typeof__ ((b)) _b = (b);              \
        _a > _b ? _b : _a; })
# define max(a,b)                               \
    ({  __typeof__((a)) _a = (a);               \
        __typeof__((b)) _b = (b);               \
        _a > _b ? _a : _b; })
#endif

#endif // PENGUINS_UTIL_H
