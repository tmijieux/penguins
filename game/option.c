#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <ctype.h>

#include <utils/math.h>

#include "option.h"

extern char *optarg;
extern int optopt;

static struct option option[] = {
    { "help", 0, NULL, 'h' },
    { "version", 0, NULL, 'v' },

    { "fish", 1, NULL, 'f' },
    { "tile", 1, NULL, 't' },
    { "map", 1, NULL, 'm' },

    { "dim", 1, NULL, 'd' },
    { "dimension", 1, NULL, 'd' },

    { 0 }
};

void print_version(void)
{
    printf("Penguin is v%d.%d.%d\n",
	   VERSION_MAJOR_NUMBER,
	   VERSION_MINOR_NUMBER,
	   VERSION_REVISION_NUMBER);
}

void print_help(void)
{
    puts("Penguin help:\n"
	 "All options are optional! ;)\n\n"

	 "\t-t N, --tile=N\n"
	 "\t\tset the tile count to N\n"
	 "\t\tdefault is 50\n\n"

	 "\t-f N, --fish=N\n"
	 "\t\tset the fish count per tile to N\n"
	 "\t\tdefault is 3\n\n"

	 "\t-d N, --dim=N, --dimension=N\n"
	 "\t\tset the dimension to N\n"
	 "\t\tdefault is 2\n\n"

	 "\t-m MAP, --map=MAP, --dimension=MAP\n"
	 "\t\tset the map to MAP\n"
	 "\t\tdefault is libmap_hexagon.so\n\n"

	 "\t-v, --version\n"
	 "\t\tshow penguins' version\n\n"

	 "\t-h, --help\n"
	 "\t\tshow this help\n"
    );
}

static int character_is_in_string(int c, char *str)
{
    int i;
    for (i = 0; str[i]; ++i)
	if (c == str[i])
	    return 1;
    return 0;
}

static char *rdstr[] = {
    "rand",
    "random",
    "randomize"
};

static int optarg_is_random(void)
{

    int l = sizeof(rdstr) / sizeof(rdstr[0]);
    for (int i = 0; i < l; i++) {
	if (!strcmp(rdstr[i], optarg))
	    return 1;
    }
    return 0;
}

static int string_is_positive_integer(const char *str)
{
    int i;
    for (i = 0; str[i]; ++i)
	if (!isdigit(str[i]))
	    return 0;
    return 1;
}

#define SET_OPT(field, max)						\
    do {								\
	if (random) {							\
	    opt->field = randomint(max);				\
	} else {							\
	    if (!string_is_positive_integer(optarg)) {			\
		fprintf(stderr, "Invalid argument to option -%c\n", c);	\
		exit(EXIT_FAILURE);					\
	    }								\
	    opt->field = atoi(optarg);					\
	}								\
    } while (0);

void parse_options(int argc, char *argv[], struct opt *opt)
{
    int c, random = 0;
    opt->tile = DEFAULT_TILE;
    opt->fish = DEFAULT_FISH;
    opt->dim = DEFAULT_DIMENSION;
    strncpy(opt->mapname, DEFAULT_MAPNAME, OPTION_BUF_SIZE);

    while ((c = getopt_long(argc, argv, "+vhf:t:d:m:", option, NULL)) != -1) {
	random = 0;
	if (character_is_in_string(c, "ftdm") && optarg_is_random())
	    random = 1;
	switch (c) {
	case 'h':
	    print_help();
	    exit(EXIT_SUCCESS);
	    break;
	case 'v':
	    print_version();
	    exit(EXIT_SUCCESS);
	    break;
	case 'f':
	    SET_OPT(fish, RAND_FISH_MAX);
	    break;
	case 't':
	    SET_OPT(tile, RAND_TILE_MAX);
	    break;
	case 'd':
	    SET_OPT(dim, RAND_DIMENSION_MAX);
	    break;
	case 'm':
	    strncpy(opt->mapname, optarg, OPTION_BUF_SIZE);
	    break;
	case '?':
	    exit(EXIT_FAILURE);
	    break;
	}
    }
}
