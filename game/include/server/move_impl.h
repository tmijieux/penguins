#ifndef PENGUINS_MOVE_IMPL_H
#define PENGUINS_MOVE_IMPL_H


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

#endif // PENGUINS_MOVE_IMPL_H
