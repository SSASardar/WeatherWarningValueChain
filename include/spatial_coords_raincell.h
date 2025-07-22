// This is the header file for the spatial_coords_raincell.c file





#ifndef SPATIAL_COORDS_RAINCELL_H
#define SPATIAL_COORDS_RAINCELL_H

#include "material_coords_raincell.h"

// Opaque defintiion of spatial raincell coordinates.
typedef struct Spatial_raincell Spatial_raincell;


// function declarations
Spatial_raincell* create_spatial_raincell(int d, double intial_x, double intial_y, double velocity);

void print_spatial_raincell(const Spatial_raincell* s_raincell, double time, const Raincell* raincell);

void free_spatial_raincell(Spatial_raincell* s_raincell);





#endif /* SPATIAL_COORDS_RAINCELL_H*/


