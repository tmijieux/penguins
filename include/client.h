#ifndef CLIENT_H
#define CLIENT_H

#include <move.h>

enum diff_type {
	MOVE,
	PENGUIN_PLACE
};

// 3 phases

// client init: chaque client récupère le plateau
void client_init(int nbTile);

// ensuite, phase 2: tour à tour, chaque joueur place un pingouin
// cette fonction doit retourner l'id de la tile ou l'ou veut placer un pingouin
int client_place_penguin();

// phase 3: "jeu": le client retourne le move choisit dans
// l'espace qui a été réservé par le serveur à l'adresse ret
// (retour par effet de bord)
// (c'est la fonction "stratégie")
void client_play(struct move *ret);

// entre chaque client_play et chaque client_place_penguin,
// les modifications sur le plateau sont envoyés à chaque client
// via la fonction send_diff :
// Il est de la responsabilité du serveur d'appeler le
//"send_diff" de chaque client a chaque fois qu'il propage une modification
// sur son plateau

// selon la valeur de dt, les différences peuvent etre:
// si dt = MOVE, un move a été effectué par un joueur
// 's' est la tile origine, et 'd' la tile destination
// modifiée

// si dt = PENGUIN_PLACE, un joueur a placé un pingouin (phase d'initialisation)
// s est la struct tile modifie, et d est inutilisé
void send_diff(enum diff_type dt, int tile_id_s, int tile_id_d);
// on peut eventuellement remplacer cette fonction par deux fonctions

#endif //CLIENT_H
