#ifndef DISPLAY_INTERFACE_H
#define DISPLAY_INTERFACE_H

#define DISPLAY_THREAD_RETVAL ((void*)-42)


void create_display_thread(int nb_dimension, int nb_tile, int nb_penguin);
int join_display_thread(void);


int display_add_move(int src, int dst);
/**
 * Permet au créateur de carte d'ajouter des pingouins à l'affichage.
 * @param tile - Tuile où se trouve le pingouin.
 * @param player - Identifiant du joueur.
 * @return int:  0 si l'ajout est valide.
 */
int display_add_penguin(int tileid, int playerid);


/**
 * return an integer refering to the registered texture
 */
int display_register_texture(const char *texture);

/**
 * return an integer refering to the registered model
 */
int display_register_model(const char *modelpath);

#endif	// DISPLAY_IFACE_H
