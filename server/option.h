#ifndef OPTION_H
#define OPTION_H

#define OPTION_BUF_SIZE 128

#define	VERSION_MAJOR_NUMBER       0
#define	VERSION_MINOR_NUMBER       2
#define VERSION_REVISION_NUMBER    0

struct opt {
    int tile;
    int fish;
    int dim;
    char mapname[OPTION_BUF_SIZE];
};

#define RAND_TILE_MAX       100
#define RAND_FISH_MAX       5
#define RAND_DIMENSION_MAX  10

#define DEFAULT_TILE         50
#define DEFAULT_FISH         3
#define DEFAULT_DIMENSION    2
#define DEFAULT_MAPNAME      "libmap_hexagon.so"

void parse_options(int argc, char *argv[], struct opt *opt);

#endif //OPTION_H
