#ifndef PENGUINS_MAP_H
#define PENGUINS_MAP_H

#include "map_interface.h"
#include "utils/graph.h"

extern struct map_methods map;

void map_module_load(const char *map_path);
void map_module_unload(void);



#endif // PENGUINS_MAP_H
