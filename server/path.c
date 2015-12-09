/**
 * @file path.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include <server/path.h>

#define PATH_MAX_ 100

int binary_dir;

#define STRING_(x) #x
#define STRING(x) STRING_(x)

/**
 * Ouverture d'un dossier.
 * @param str - Dossier à ouvrir.
 */
void path_init(const char *str)
{
#ifndef TEST
    char *path = strndup(str, PATH_MAX_);
    int l = strlen(path);
    while (path[l] != '/' && l>=0) // removes the executable name
	path[l--] = 0;             // keep only the dirname
#else
    char path[] = STRING(SERVER_INSTALL_PATH);
#endif
    if ((binary_dir = open(path, O_DIRECTORY)) == -1) {
	perror(path);
	exit(EXIT_FAILURE);
    }
#ifndef TEST
    free(path);
#endif
}

/**
 * Fermeture du dossier.
 */
void path_exit(void)
{
    close(binary_dir);
}
