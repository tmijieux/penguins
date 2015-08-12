#include <stdio.h>
#include <stdlib.h>
#include <utils/log.h>
#include <stdarg.h>

/**
 * @file log.c
 */

static int init_done = 0;
static int log_level;
static FILE *log_file = NULL;

/**
 * Initialisation du module de log.
 * @param level - Niveau de log.\n
 * PRINT_LOG__ -> Affichage des logs de niveau PRINT.\n
 * INFO_LOG__ -> Affichage des logs de niveau INFO + au dessus.\n
 * ERROR_LOG__ -> Affichage des logs de niveau ERROR + au dessus.\n
 * WARN_LOG__ -> Affichage des logs de niveau WARN + au dessus.\n
 * DEBUG_LOG__ -> Affichage des logs de niveau DEBUG + au dessus.\n
 * ALL_LOG__ -> Affichage de tous les logs.\n
 * OFF_LOG__ -> Pas d'affichage.
 * @param log_filename - Fichier dans lequel écrire les logs.
 */
void log_init(enum LOG_LEVEL level, const char *log_filename)
{
    log_level = level;
    if (log_file == NULL) {
	log_file = fopen(log_filename, "w");
	if (log_file == NULL) {
	    perror(log_filename);
	    exit(EXIT_FAILURE);
	}
    }
    init_done = 1;
}

/**
 * Changer le niveau des logs.
 * @param level - Niveau de log.\n
 * PRINT_LOG__ -> Affichage des logs de niveau PRINT.\n
 * INFO_LOG__ -> Affichage des logs de niveau INFO + au dessus.\n
 * ERROR_LOG__ -> Affichage des logs de niveau ERROR + au dessus.\n
 * WARN_LOG__ -> Affichage des logs de niveau WARN + au dessus.\n
 * DEBUG_LOG__ -> Affichage des logs de niveau DEBUG + au dessus.\n
 * ALL_LOG__ -> Affichage de tous les logs.\n
 * OFF_LOG__ -> Pas d'affichage.
 */
void log_set_level(enum LOG_LEVEL level)
{
    log_level = level;
}

/**
 * Afficher un log.
 * @param level - Niveau de log.\n
 * PRINT_LOG__ -> Affiché si le niveau correspond à la configuration.\n
 * INFO_LOG__  -> Affiché si le niveau correspond à la configuration.\n
 * ERROR_LOG__ -> Affiché si le niveau correspond à la configuration.\n
 * WARN_LOG__ -> Affiché si le niveau correspond à la configuration.\n
 * DEBUG_LOG__ -> Affiché si le niveau correspond à la configuration.\n
 * NEVER_LOG__ -> Jamais affiché.
 * @param format - Pattern de la chaine de caractère à afficher.
 * @param ... - Variables correspondantes au format.
 */
void log_print(enum LOG_LEVEL level, const char *format, ...)
{
    if (!init_done || log_level == OFF_LOG__ || level == NEVER_LOG__)
	return;
    va_list ap;
    va_start(ap, format);
    if (level <= log_level)
	vprintf(format, ap);
    va_start(ap, format);
    vfprintf(log_file, format, ap);
}

/**
 * Fermeture du module log.
 */
void log_exit(void)
{
    log_level = OFF_LOG__;
    init_done = 0;
    fclose(log_file);
    log_file = NULL;
}
