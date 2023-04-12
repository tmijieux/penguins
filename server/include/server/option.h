#ifndef OPTION_H
#define OPTION_H


#define MAP_BUFFER_SIZE 128

struct opt {
    int tile;
    int fish;
    int dim;
    char map[MAP_BUFFER_SIZE];
};

void parse_options(int argc, char *argv[], struct opt *opt);

#endif //OPTION_H
