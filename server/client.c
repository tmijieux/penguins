/**
 * @file
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdarg.h>
#include <fcntl.h>

#include <server/client.h>
#include <server/path.h>
#include <utils/list.h>

static int client_count = 0;
static void *client_handler[MAX_CLIENT] = { NULL };
static struct list *name = NULL;

static void client_load_available_clients(void);

/**
 * Stop le programme.
 * @param format - Les options souhaitées.
 * @param ... - Autres paramètres.
 */
static void die(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    vfprintf(stderr, format, ap);
    exit(EXIT_FAILURE);
}

/**
 * Informe si le fichier est un .so
 * @param ep - Fichier à tester.
 * @return int - 1 Si c'est un .so , 0 sinon.
 */
static int is_shared_library(struct dirent *ep)
{
    size_t l = strlen(ep->d_name);
    if (l < 3)
        return 0;
    return (strcmp(".so", ep->d_name + l - 3) == 0);
}

/**
 * Initialisation du module client, 
 * chargements des différents clients.
 */
void client_init(void)
{
    name = list_create(LIST_FREE_MALLOCD_ELEMENT__);
    client_load_available_clients();
}

/**
 * Libération de la mémoire occupée par le module client.
 * Déchargement des clients.
 */
void client_exit(void)
{
    for (int i = 0; i < client_count; i++)
	dlclose(client_handler[i]);
    client_count = 0;
    list_destroy(name);
    name = NULL;
}

/**
 * Charger les clients disponibles.
 */
static void client_load_available_clients(void)
{
    if (client_handler[0] != NULL)
        return;
    int fd = openat(binary_dir, "./clients/", O_DIRECTORY);
    DIR *client_dir = fdopendir(fd);
    if (client_dir == NULL) {
        perror("(binary dir)/clients/");
        exit(EXIT_FAILURE);
    }
    struct dirent *ep;
    while ((ep = readdir(client_dir))) {
        if (is_shared_library(ep)) {
            printf("== client found: %s ==\n", ep->d_name);
            void *handle = dlopen(ep->d_name, RTLD_NOW);
            if (handle == NULL) {
                puts(dlerror());
                exit(EXIT_FAILURE);
            }
            client_handler[client_count++] = handle;
	    int j = strlen(ep->d_name);
	    char *tmpstr = malloc(j+1);
            strncpy(tmpstr, ep->d_name, j+1);
            tmpstr[j-3] = 0;
            list_insert_element(name, client_count, tmpstr);
        }
    }
    closedir(client_dir);
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
 * Récupération d'une fonction d'un client.
 * @param client_id - Identifiant du client.
 * @param method_name - Nom de la fonction.
 * @return void * - Pointeur de la fonction.
 */
void *client_get_method(int client_id, const char *method_name)
{
    void *handle = client_handler[client_id];
    void *pf = dlsym(handle, method_name);
    if (pf == NULL)
	die(dlerror());
    return pf;
}

/**
 * Obtenir le nom d'un client.
 * @param client - Identifiant du client.
 * @param const char * - Nom du client.
 */
const char *client_get_name(int client)
{
    return list_get_element(name, client+1);
}
