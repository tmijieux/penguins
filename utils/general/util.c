#include <string.h>
#include <utils/util.h>

/**
 * Informe si le fichier a un extension donné
 * @param filename - Fichier à tester.
 * @param extension - l'extension (avec le . au début)
 * @return int - 1 Si c'est l'extension correspond , 0 sinon.
 */
static int file_have_extension(const char *filename, const char *extension)
{
    size_t l = strlen(filename);
    size_t e = strlen(extension);
    if (l < e)
        return 0;
    return (strcmp(extension, filename + l - e) == 0);
}

/**
 * Informe si le fichier est un .so
 * @param filename - Fichier à tester.
 * @return int - 1 Si c'est un .so , 0 sinon.
 */
int is_shared_library(const char *filename)
{
    return file_have_extension(filename, ".so");
}
