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

	printf("I just crafted spatial raincell %d\n\n", s_raincell->id);

	return s_raincell;	
}


void print_spatial_raincell(const Spatial_raincell* s_raincell, double time, const Raincell* raincell){

	if(raincell != NULL && raincell_get_id(raincell)== s_raincell->id){
		printf("time is: %.2lf seconds\n \n", time);
		printf("core top height is: %.2lf meters\n \n", raincell_get_top_height_core(raincell, time));
		printf("===================================\n \n");
	
		//printf("Raincell %d: \n at time %.2lf seconds (%.2lf minutes, %.2lf hours) \n is at point (x,y) = (%.2lf, %.2lf) \n and has cell heights of (h_stratiform, h_core) = (%.2lf, %.2lf)\n", s_raincell->id, time, time/60, time/3600, s_raincell->initial_x + time* s_raincell->dx,s_raincell->initial_y + time* s_raincell->dy, raincell_get_top_height_stratiform(raincell), raincell_get_top_height_core(raincell, time));


	
	} /*else if (raincell_get_id(raincell)!=s_raincell_id) {

		printf("Error: the spatial and material raincell id's don't match. Check you are calling the right functions\n\n")

	}*/
}


void print_path_spatial_raincell(const Spatial_raincell* s_raincell){
	printf("Spatial raincell %d:\n 		The parametric equations for the path centre of cell (x(t), y(t)) is:\n 		x(t) = %.2lf * t + %.2lf\n		y(t) = %.2lf * t + %.2lf\n\n", s_raincell->id, s_raincell->dx, s_raincell->initial_x, s_raincell->dy, s_raincell->initial_y);

}




void free_spatial_raincell(Spatial_raincell* s_raincell){
	int id_d = s_raincell->id;
	free(s_raincell);
	printf("I just obliterated (freed) spatial raincell %d\n\n", id_d);
}

void get_position_raincell(double time, const Spatial_raincell* cell, double *x_out, double *y_out) {
    if (cell && x_out && y_out) {
        *x_out = cell->initial_x + time * cell->dx;
        *y_out = cell->initial_y + time * cell->dy;
    }
}

