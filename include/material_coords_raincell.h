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

#define MAX_RAINCELLS 2

// Opaque definition of raincell structure
typedef struct Raincell {

	int id;
	double radius_core;
	double radius_stratiform;
	double offset_centre_core;
	double top_height_stratiform;
	//double top_height_core;
	double max_top_height_core;
	double onset_time_growth;
	double stop_time_growth;
	double onset_time_decay;
	double stop_time_decay;
} Raincell;

	
Raincell* create_raincell(int id, double relative_size_core, double radius_stratiform, double relative_offset, double top_height_stratiform,/*  double top_height_core,*/ double max_top_height_core, double onset_time_growth, double stop_time_growth, double onset_time_decay, double stop_time_decay);


//global registries:

extern Raincell* raincell_list[MAX_RAINCELLS];
extern int raincell_count;






void print_raincell(const Raincell* raincell);

void free_raincell(Raincell* raincell);

// Getter functions
int raincell_get_id(const Raincell* raincell);
double raincell_get_radius_core(const Raincell* raincell);
double raincell_get_radius_stratiform(const Raincell* raincell);
double raincell_get_offset_centre_core(const Raincell* raincell);
double raincell_get_top_height_stratiform(const Raincell* raincell);
double raincell_get_top_height_core(const Raincell* raincell, double time);
//double raincell_get_time(const Raincell* raincell);






// Declarations of public functions
void internal_geometry_raincell(void);
const Raincell* find_raincell_by_id_ONLY(int idA);




#endif /* MATERIAL_COORDS_RAINCELL_H */
