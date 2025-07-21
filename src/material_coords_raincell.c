// This is a .c file
//
//
//
//
//





// include statements: 
#include "material_coords_raincell.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "true_vertical_profile.h"
#include "common.h"




// Structure of a raincell

struct Raincell {
	int id;
	double radius_core;
	double radius_stratiform;
	double offset_centre_core;
	double top_height_stratiform;
	double top_height_core;
	double time;
};


Raincell* create_raincell(int id, double relative_size_core, double radius_stratiform, double relative_offset, double top_height_stratiform, double top_height_core, double time) {

	Raincell* raincell = malloc(sizeof(Raincell));
	raincell->id =  id;
	raincell->radius_core = relative_size_core*radius_stratiform;
	raincell->radius_stratiform = radius_stratiform;
	raincell->offset_centre_core = relative_offset*radius_stratiform;
	raincell->top_height_stratiform = top_height_stratiform;
	raincell->top_height_core = top_height_core;
	raincell->time = time;
	
	return raincell;

}



void print_raincell(const Raincell* raincell){
	printf("Raincell %d has: \n    a core with radius %.2lf,\n    a stratiform area of radius %.2lf,\n    centred aroung point (0,0) in material coordinates,\n    with heights of %.2lf and %.2lf of the core and stratiform areas\n\n",raincell->id, raincell->radius_core, raincell->radius_stratiform, raincell->top_height_core, raincell->top_height_stratiform);
}

void free_raincell(Raincell* raincell){
	printf("I am destroying raincell %d\n", raincell->id);
	free(raincell);
	printf("destroyed!! >:)\n");
}

// Getters for Raincell fields
int raincell_get_id(const Raincell* raincell) {
    return raincell->id;
}

double raincell_get_radius_core(const Raincell* raincell) {
    return raincell->radius_core;
}

double raincell_get_radius_stratiform(const Raincell* raincell) {
    return raincell->radius_stratiform;
}

double raincell_get_offset_centre_core(const Raincell* raincell) {
    return raincell->offset_centre_core;
}

double raincell_get_top_height_stratiform(const Raincell* raincell) {
    return raincell->top_height_stratiform;
}

double raincell_get_top_height_core(const Raincell* raincell) {
    return raincell->top_height_core;
}

double raincell_get_time(const Raincell* raincell) {
    return raincell->time;
}


