#ifndef PENGUINS_SYMBOL_VISIBILITY_H
#define PENGUINS_SYMBOL_VISIBILITY_H

#ifdef __GNUC__
# define __internal  __attribute__ ((visibility ("internal")))
# define __export  __attribute__ ((visibility ("default")))
#else
# define __internal
# define __export
#endif

#ifndef _WIN32
# define __declspec(x)
#endif

#endif // PENGUINS_SYMBOL_VISIBILITY_H
