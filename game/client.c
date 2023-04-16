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
#endif


#include "server/client.h"
#include "utils/list.h"


static int clients_initialized = 0;
static int client_count = 0;
static struct client clients[MAX_CLIENT] = { NULL };

static void client_load_available_clients(void);

/**
 * Informe si le fichier est un .so
 * @param ep - Fichier à tester.
 * @return int - 1 Si c'est un .so , 0 sinon.
 */
static int is_shared_library(const char *filename)
{
    size_t l = strlen(filename);

#ifdef _WIN32
    if (l < 5) {
        return 0;
    }
    return (strcmp(".dll", filename + l - 4) == 0);
#else
    if (l < 4) {
        return 0;
    }
    return (strcmp(".so", filename + l - 3) == 0);

#endif
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
#ifdef __unix__
	    dlclose(clients[i].dl_handle);
#else
        FreeLibrary(clients[i].dl_handle);
#endif
    }
    client_count = 0;
}

/**
 * Charger les clients disponibles.
 */
#ifndef _WIN32
static void client_load_available_clients(void)
{
    if (clients_initialized) {
        return;
    }
    clients_initialized = 1;

    DIR *client_dir = opendir("./clients");
    if (client_dir == NULL) {
        perror("(binary dir)/clients/");
        exit(EXIT_FAILURE);
    }

    int num_client = 0;
    struct dirent *ep;
    while ((ep = readdir(client_dir))) {
        if (is_shared_library(ep->d_name)) {
            printf("== client found: %s ==\n", ep->d_name);
            ++num_client;
        }
    }
    closedir(client_dir);

    client_dir = opendir("./clients");
    if (client_dir == NULL) {
        perror("(binary dir)/clients/");
        exit(EXIT_FAILURE);
    }

    int i = 0;
    while ((ep = readdir(client_dir))) {
        if (!is_shared_library(ep->d_name)) {
            continue;
        }
        Client *cli = &clients[i++];
        cli->dl_handle = dlopen(ep->d_name, RTLD_NOW);
        if (cli->dl_handle == NULL) {
            puts(dlerror());
            exit(EXIT_FAILURE);
        }
        typedef void (*register_t)(struct client_methods*);
        register_t client_register = dlsym(cli->dl_handle, "client_register");
        if (client_register == NULL)  {
            puts(dlerror());
            exit(EXIT_FAILURE);
        }
        client_register(&cli->methods);
    }

    closedir(client_dir);
    client_count = num_client;
}
#endif  // _WIN32


#ifdef _WIN32
static void iter_on_directory(void (*callback)(int id, const char* path))
{

    WIN32_FIND_DATA FindFileData;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    // DWORD dwError;

    const char dirSpec[200] = ".\\clients\\*";
    int id = 0;

    hFind = FindFirstFile(dirSpec, &FindFileData);
    if (hFind == INVALID_HANDLE_VALUE) {
        perror("Could not open client player module direction `./clients`");
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

static void count_client(int _id, const char*name)
{
    (void)_id;
    printf("== client found: %s ==\n", name);
    ++client_count;
}

static void load_client(int id, const char* name)
{
    printf("id=%d file=%s\n", id, name);


    const char *basepath = ".\\clients\\";
    char path[512] = { 0 };
    int lenBase = strlen(basepath);
    int lenName = strlen(name);

    strncpy(path, basepath, lenBase);
    strncpy(path + lenBase, name, min(lenName+1, 512 - lenBase));
    path[511] = 0;

    Client* cli = &clients[id];
    cli->dl_handle = LoadLibrary(path);
    if (cli->dl_handle == NULL) {
        printf("Error while loading client module %s\n", path);
        exit(EXIT_FAILURE);
    }

    typedef void (*register_t)(struct client_methods*);
    register_t client_register = GetProcAddress(cli->dl_handle, "client_register");
    if (client_register == NULL) {
        printf("Error while loading function `client_register` for module %s\n", path);
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
#endif

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

Client *client_get(int id) {
    if (id >= MAX_CLIENT) {
        return NULL;
    }
    return &clients[id];
}
