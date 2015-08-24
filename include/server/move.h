#ifndef SERVER_MOVE_H
#define SERVER_MOVE_H

#include <utils/graph.h>
#include <move.h>

/**
 * décrit le coup d'un joueur
 */
struct move {
    /// tuile de départ du pingouin
    int orig;

     /// direction du déplacement
    int direction;

     /// nombre de tuile sauté
    int jump_count;
};

#endif //SERVER_MOVE_H
