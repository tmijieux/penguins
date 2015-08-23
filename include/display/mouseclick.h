#ifndef MOUSECLICK_IFACE_H
#define MOUSECLICK_IFACE_H

enum mc_object_type {
    MC_TILE,
    MC_PENGUIN
};

struct mouseclick {
    int validclick;
    enum mc_object_type t;
    int tile_id;
    int peng_id;
};

int display_mc_get(struct mouseclick *mc);
// return the id of the tile where was located the object
// that the player clicked one
// either a tile or a penguin.


void display_mc_init(int (*coord_on_tile)(double x, double z),
		     double z_shoes, double z_feet, double z_head);
/*

  side view:

  *-----------------*  z_head  
  |                 |
  |                 |
  |                 |
  |     penguin     |
  |                 |
  |                 |
  |                 |
  *-----------------*  z_feet
  |                 |
  |      tile       |
  |                 |
  |                 | 
  *-----------------*  z_shoes


  top view:
  tile:
  *-----------------*  
  |             X P |
  |                 |
  |       X O       |          O is origin of the tile
  |                 |          P is any point in the world 
  *-----------------*               pointed out by the user
  coord_on_tile :
         (x, z) = OP vector (projected on x and z axis)
	 must returns 1 if  P is on tile, 0 otherwise
	 for an !!unscaled!! tile
  
*/

#endif //MOUSECLICK_IFACE_H
