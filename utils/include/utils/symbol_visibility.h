#ifndef PENGUINS_SO_H
#define PENGUINS_SO_H

#ifdef __GNUC__

#define __internal  __attribute__ ((visibility ("internal")))
#define __export  __attribute__ ((visibility ("default")))

#else

#define __internal
#define __export

#endif

#endif // PENGUINS_SO_H
