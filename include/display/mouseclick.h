#ifndef MOUSECLICK_H
#define MOUSECLICK_H

enum mc_object_type {
    MC_TILE,
    MC_PENGUIN
};

struct mouseclick {
    int validclick;
    enum mv_object_type t;
    int id;
};

int display_mc_get(struct mouseclick *mc);
// return the id of the tile where was located the object
// that the player clicked one
// either a tile or a penguin.


#endif //MOUSECLICK_H
