// This is the .h header file of the material_coords_raincell.c file
//
//
//
//
//
//
//
//






#ifndef MATERIAL_COORDS_RAINCELL_H
#define MATERIAL_COORDS_RAINCELL_H


// Opaque definition of raincell structure
typedef struct Raincell Raincell;


Raincell* create_raincell(int id, double relative_size_core, double radius_stratiform, double relative_offset, double top_height_stratiform, double top_height_core, double time);


void print_raincell(const Raincell* raincell);

void free_raincell(Raincell* raincell);








// Declarations of public functions
void internal_geometry_raincell(void);

#endif /* MATERIAL_COORDS_RAINCELL_H */
