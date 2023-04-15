#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <dlfcn.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>

#include "server/client.h"
#include "server/path.h"
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
static int is_shared_library(struct dirent *ep)
{
    size_t l = strlen(ep->d_name);
    if (l < 3) {
        return 0;
    }
    return (strcmp(".so", ep->d_name + l - 3) == 0);
}

/**
 * Initialisation du module client,
 * chargements des différents clients.
 */
void client_module_init(void)
{
    client_load_available_clients();
}

/**
 * Libération de la mémoire occupée par le module client.
 * Déchargement des clients.
 */
void client_module_exit(void)
{
    for (int i = 0; i < client_count; i++) {
	dlclose(clients[i].dl_handle);
    }
    client_count = 0;
}

/**
 * Charger les clients disponibles.
 */
static void client_load_available_clients(void)
{
    if (clients_initialized) {
        return;
    }
    clients_initialized = 1;

    int fd = openat(binary_dir, "./clients/", O_DIRECTORY);
    DIR *client_dir = fdopendir(fd);
    if (client_dir == NULL) {
        perror("(binary dir)/clients/");
        exit(EXIT_FAILURE);
    }

    int num_client = 0;
    struct dirent *ep;
    while ((ep = readdir(client_dir))) {
        if (is_shared_library(ep)) {
            printf("== client found: %s ==\n", ep->d_name);
            ++num_client;
        }
    }
    closedir(client_dir);

    fd = openat(binary_dir, "./clients/", O_DIRECTORY);
    client_dir = fdopendir(fd);
    if (client_dir == NULL) {
        perror("(binary dir)/clients/");
        exit(EXIT_FAILURE);
    }

    int i = 0;
    while ((ep = readdir(client_dir))) {
        if (!is_shared_library(ep)) {
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
    puts("clients_load seems to have ended successfully");
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

Client *client_get(int id) {
    if (id >= MAX_CLIENT) {
        return NULL;
    }
    return &clients[id];
}
