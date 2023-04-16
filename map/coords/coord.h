#ifndef PENGUINS_COORD_H
#define PENGUINS_COORD_H

// Retourne la longueur du côté de la grille
size_t coord_get_length_side(int number_tiles, int number_dimensions);

// Place les coordonnées du sommet #id dans la grille dans un tableau
// coord de la forme [c(0), ..., c(number_dimensions-1)]
// Place -1 en première coordonnée si l'id est invalide
void coord_get_coordinates_from_id(int id, int number_tiles, size_t length,
				   int number_dimensions, int *coord);

// Retourne l'id du sommet à partir des coordonnées données dans un tableau
// [c(0), ..., c(number_dimensions-1)]
// Retourne -1 si les coordonnées sont invalides
int coord_get_id_from_coordinates(int *coord, size_t length,
				  int number_dimensions);

#endif // PENGUINS_COORD_H
