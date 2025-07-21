// THis is a .c file containing the spatial coordinate descriptions of the raincell. 






// include statements
#include "spatial_coords_raincell.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "material_coords_raincell.h"






// Structure of spatial raincell

struct Spatial_raincell {
	int id;
	double initial_x;
	double initial_y;
	double dx;
	double dy;
};

Spatial_raincell* create_spatial_raincell(int id, double initial_x, double initial_y, double velocity) {
	Spatial_raincell* s_raincell = malloc(sizeof(Spatial_raincell));
	s_raincell->id = id;
	s_raincell->initial_x = initial_x;
	s_raincell->initial_y = initial_y;
	s_raincell->dx = velocity;
	s_raincell->dy = 0;

	return s_raincell;	
}


void print_s_raincell(const Spatial_raincell* s_raincell, double time, const Raincell* raincell){

	if(raincell != NULL && raincell_get_id(raincell)== s_raincell->id){
	
		printf("Raincell %d: \n is at point (x,y) = (%.2lf, %.2lf) \n and has cell heights of (h_stratiform, h_core) = (%.2lf, %.2lf)\n", s_raincell->id, s_raincell->initial_x + time* s_raincell->dx,s_raincell->initial_y + time* s_raincell->dy, raincell_get_top_height_stratiform(raincell), raincell_get_top_height_core(raincell));
	
	}
}

void free_spatial_raincell(Spatial_raincell* s_raincell){
	printf("I am destroying spatial raincell %d\n", s_raincell->id);
	free(s_raincell);
	printf("destroyed!!! >:)\n");
}

