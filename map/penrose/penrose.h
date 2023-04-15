#ifndef MAP_PENROSE_H
#define MAP_PENROSE_H
// Init the penrose module
void penrose_init(int nb_lozenge, int dimension);
// Give the number of layers in each dimension > 2
int penrose_get_layer();
// Give the maximum number of lozenges of a layer
int penrose_get_surface();
// Give the angle of a lozenge between AC and x axis
int penrose_get_angle(int id);
// Give the id of the #direction neighbor of lozenge #id
int penrose_get_neighbor(int id, int direction);
// Give the destination id from a move
int penrose_get_id_from_move(int id, int *direction);
// Give the type (0 or 1) of the lozenge #id
int penrose_get_type(int id);
// Fill coord[3] with the coordinates of lozenge "id
void penrose_get_coordinates_from_id(int id, float coord[3]);
// Free the penrose module
void penrose_free(void);

#endif // MAP_PENROSE_H
