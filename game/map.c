#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils/graph.h"
#include "utils/module_loading.h"

#include "penguins/map_interface.h"
#include "server/map.h"


peng_libhandle_t handle;
struct map_methods map;

/**
 * Chargement du module map.
 * @param map_path - Chemin d'accès au module.
 */
void map_module_load(const char *map_path)
{
    typedef void (*register_t)(struct map_methods*);
    register_t map_register;

    handle = peng_load_library(map_path);
    if (handle == NULL) {
        peng_lib_handle_error("library", map_path);
        exit(EXIT_FAILURE);
    }

    map_register = peng_get_proc_address(handle, "map_register");
    if (map_register == NULL) {
        peng_lib_handle_error("function", "map_register");
        exit(EXIT_FAILURE);
    }

    map_register(&map);
}

/**
 * Déchargement du module map.
 */
void map_module_unload(void)
{
    if (map.exit != NULL) {
        map.exit();
    }
    memset(&map, 0, sizeof(map));
    peng_free_library(handle);
    handle = NULL;
}
