#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <sys/types.h>
#include <fcntl.h>

#ifdef _WIN32
# include <Windows.h>
#else
# include <dirent.h>
# include <dlfcn.h>
# define min(a,b)                               \
    ({ __typeof__ (a) _a = (a);                 \
        __typeof__ (b) _b = (b);                \
        _a > _b ? _b : _a; })
#endif


#include "server/client.h"
#include "utils/list.h"


static int clients_initialized = 0;
static int client_count = 0;
static struct client clients[MAX_CLIENT] = { NULL };

static void client_load_available_clients(void);

/**
 * Teste si le fichier est un module chargeable
 * @param ep - Fichier à tester.
 * @return int - 1 Si c'est un module , 0 sinon.
 */
static int is_shared_library(const char *filename)
{
    size_t L = strlen(filename);
    size_t N = strlen(PENG_MODULE_EXTENSION);

    if (L <= N) {
        return 0;
    }
    return (strcmp(PENG_MODULE_EXTENSION, filename + L - N) == 0);
}

/**
 * Initialisation du module client,
 * chargements des différents clients.
 */
void client_module_init(void)
{
    client_load_available_clients();
    if (client_count == 0) {
        puts("WARNING: No client player module was loaded!");
    }
    else {
        puts("clients_load seems to have ended successfully");
    }

}

/**
 * Libération de la mémoire occupée par le module client.
 * Déchargement des clients.
 */
void client_module_exit(void)
{
    for (int i = 0; i < client_count; i++) {
        peng_free_library(clients[i].handle);
    }
    client_count = 0;
}

/**
 * Charger les clients disponibles.
 */


#ifdef _WIN32
static void iter_on_directory(void (*callback)(int id, const char* path))
{
    WIN32_FIND_DATA FindFileData;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    // DWORD dwError;

    const char *dirSpec = ".\\clients\\*";
    int id = 0;

    hFind = FindFirstFile(dirSpec, &FindFileData);
    if (hFind == INVALID_HANDLE_VALUE) {
        perror("Could not open client player module directory `./clients`");
        exit(EXIT_FAILURE);
    }
    if (is_shared_library(FindFileData.cFileName))
    {
        callback(id++, FindFileData.cFileName);
    }
    while (FindNextFile(hFind, &FindFileData) != 0) {
        if (is_shared_library(FindFileData.cFileName))
        {
            callback(id++, FindFileData.cFileName);
        }
    }
    FindClose(hFind);
}
#else
static void iter_on_directory(void (*callback)(int id, const char *path))
{
    DIR *client_dir = opendir("./clients");
    if (client_dir == NULL) {
        perror("Could not open directory ./clients/");
        exit(EXIT_FAILURE);
    }

    int id = 0;
    struct dirent *ep;
    while ((ep = readdir(client_dir)))
    {
        if (is_shared_library(ep->d_name))
        {
            callback(id++, ep->d_name);
        }
    }
    closedir(client_dir);
}
#endif  // _WIN32

static void count_client(int _id, const char*name)
{
    (void)_id;
    printf("== client found: %s ==\n", name);
    ++client_count;
}

static void load_client(int id, const char* name)
{
    #ifdef _WIN32
    const char *basepath = ".\\clients\\";
    #else
    const char *basepath = "./clients/";
    #endif
    char path[512] = { 0 };
    int lenBase = strlen(basepath);
    int lenName = strlen(name);

    strncpy(path, basepath, lenBase);
    strncpy(path + lenBase, name, min(lenName+1, 512 - lenBase));
    path[511] = 0;

    Client* cli = &clients[id];
    cli->handle = peng_load_library(path);
    if (cli->handle == NULL) {
        peng_lib_handle_error("client module", path);
        exit(EXIT_FAILURE);
    }

    typedef void (*register_t)(struct client_methods*);
    register_t client_register = peng_get_proc_address(cli->handle, "client_register");
    if (client_register == NULL) {
        peng_lib_handle_error("function `client_register` for module", path);
        exit(EXIT_FAILURE);
    }
    client_register(&cli->methods);
}

static void client_load_available_clients(void)
{
    if (clients_initialized) {
        return;
    }
    clients_initialized = 1;

    iter_on_directory(count_client);
    iter_on_directory(load_client);

}

/**
 * Obtenirle nombre de clients chargés.
 * @return int - Nombre de clients.
 */
int client_get_client_count(void)
{
    return client_count;
}


/**
 * Obtenir le nom d'un client.
 * @param client - Identifiant du client.
 * @param const char * - Nom du client.
 */
const char *client_get_name(int id)
{
    return clients[id].methods.name;
}

Client *client_get(int id)
{
    if (id >= MAX_CLIENT) {
        return NULL;
    }
    return &clients[id];
}
