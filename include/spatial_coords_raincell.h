// This is the header file for the spatial_coords_raincell.c file





#ifndef SPATIAL_COORDS_RAINCELL_H
#define SPATIAL_COORDS_RAINCELL_H

#include "common.h"
#include "material_coords_raincell.h"

// Opaque defintiion of spatial raincell coordinates.
typedef struct Spatial_raincell {
	int id;
	double initial_x;
	double initial_y;
	double dx;
	double dy;
} Spatial_raincell;



//global registries:

extern Spatial_raincell* s_raincell_list[MAX_RAINCELLS];





// function declarations
Spatial_raincell* create_spatial_raincell(int d, double intial_x, double intial_y, double velocity);

// Finder function
const Spatial_raincell* find_spatial_raincell_by_id_ONLY(int idA);


void print_spatial_raincell(const Spatial_raincell* s_raincell, double time, const Raincell* raincell);

void print_path_spatial_raincell(const Spatial_raincell* s_raincell);

void free_spatial_raincell(Spatial_raincell* s_raincell);

Point* get_position_raincell(double time, const Spatial_raincell* cell);

Bounding_box* create_BoundingBox_for_s_raincell(const Spatial_raincell* s_raincell, double time,  const Raincell* raincell);





#endif /* SPATIAL_COORDS_RAINCELL_H*/


