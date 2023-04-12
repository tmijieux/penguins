#include <stdio.h>
#include <dlfcn.h>
#include <string.h>

#include "map_interface.h"
#include "server/map.h"
#include "utils/graph.h"

static void *handle;
struct map_methods map;

/**
 * Chargement du module map.
 * @param map_path - Chemin d'accès au module.
 */
void map_module_load(const char *map_path)
{
    if ((handle = dlopen(map_path, RTLD_NOW)) == NULL) {
	puts(dlerror());
	exit(EXIT_FAILURE);
    }
    typedef void (*register_t)(struct map_methods*);

    register_t map_register = dlsym(handle, "map_register");
    if (map_register == NULL) {
	puts(dlerror());
	exit(EXIT_FAILURE);
    }
    map_register(&map);
}

/**
 * Déchargement du module map.
 */
void map_module_unload(void)
{
    dlclose(handle);
    if (map.exit != NULL) {
        map.exit();
    }
    memset(&map, 0, sizeof(map));
}
