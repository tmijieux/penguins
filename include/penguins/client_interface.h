#ifndef PENGUINS_CLIENT_INTERFACE_H
#define PENGUINS_CLIENT_INTERFACE_H

#include "utils/symbol_visibility.h"

struct move;
struct mouseclick;

enum diff_type {
    MOVE,
    PENGUIN_PLACE
};
typedef struct game_methods {
    int (*get_current_player_id)(void);
    int (*move_is_valid)(int tile, int, int);
    void (*set_move)(struct move*, int, int, int);
    int (*get_tile_nb_fishes)(int tile_id);
    const int* (*get_tile_neighbours)(int tile_id);
    int (*get_tile_neighbours_count)(int tile_id);
    int (*get_tile_player)(int tile_id);
    int (*get_tile_nb_direction)(int tile_id);

    int (*get_nb_player)(void);
    int (*get_nb_penguin_player)(int playerid);

    int (*get_mouseclick_from_display)(struct mouseclick* out);
} game_methods_t;

typedef struct client_methods {
    // client init: chaque client récupère le plateau
    void (*init)(int nbTile, game_methods_t *game);

    // ensuite, phase 2: tour à tour, chaque joueur place un pingouin
    // cette fonction doit retourner l'id de la tile ou l'ou veut placer un pingouin
    int (*place_penguin)(void);

    // phase 3: "jeu": le client retourne le move choisit dans
    // l'espace qui a été réservé par le serveur à l'adresse ret
    // (retour par effet de bord)
    // (c'est la fonction "stratégie")
    void (*play)(struct move*);

    // entre chaque client_play et chaque client_place_penguin,
    // les modifications sur le plateau sont envoyés à chaque client
    // via la fonction send_diff :
    // Il est de la responsabilité du serveur d'appeler le
    // "send_diff" de chaque client a chaque fois qu'il propage une modification
    // sur son plateau

    // selon la valeur de dt, les différences peuvent etre:
    // si dt = MOVE, un move a été effectué par un joueur
    // 's' est la tile origine, et 'd' la tile destination
    // modifiée

    // si dt = PENGUIN_PLACE, un joueur a placé un pingouin (phase d'initialisation)
    // s est la struct tile modifie, et d est inutilisé
    void (*send_diff)(enum diff_type dt,int src,int dst);

    void (*exit)(void);
    char name[128];
} client_methods_t;


__declspec(dllexport)
void client_register(client_methods_t*);

#endif // PENGUINS_CLIENT_INTERFACE_H
