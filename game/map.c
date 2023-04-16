#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
# include <Windows.h>
#else
# include <dlfcn.h>
#endif


#include "penguins/map_interface.h"
#include "server/map.h"
#include "utils/graph.h"

#ifdef _WIN32
HINSTANCE handle;
#else
static void* handle;

#endif
struct map_methods map;

/**
 * Chargement du module map.
 * @param map_path - Chemin d'accès au module.
 */
void map_module_load(const char *map_path)
{
    typedef void (*register_t)(struct map_methods*);
    register_t map_register;
    #ifdef _WIN32
    handle = LoadLibrary(map_path);
    if (handle == NULL) {
        char buf[256] = {0};
        GetCurrentDirectory(255, buf);
        printf("Error when loading library %s cur_dir=%s\n", map_path, buf);
        exit(EXIT_FAILURE);
    }
    #else
    handle = dlopen(map_path, RTLD_NOW);
    if (handle == NULL) {
        puts(dlerror());
        exit(EXIT_FAILURE);
    }
    #endif

    #ifdef _WIN32
    map_register = GetProcAddress(handle, "map_register");
    if (map_register == NULL) {
        printf("Error when loading function map_register\n");
        exit(EXIT_FAILURE);
    }
    #else
    map_register = dlsym(handle, "map_register");
    if (map_register == NULL) {
        puts(dlerror());
        exit(EXIT_FAILURE);
    }
    #endif

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
#ifdef __unix__
    dlclose(handle);
#else
    FreeLibrary(handle);
#endif
}
