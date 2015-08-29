#ifndef SO_H
#define SO_H

#ifdef __GNUC__
#define __so_local  __attribute__ ((visibility ("internal")))
#define __export  __attribute__ ((visibility ("default")))
#else
#define __so_local
#define __export 
#endif

#endif //SO_H
