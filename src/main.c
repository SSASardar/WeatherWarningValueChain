// this is the main.c file
//
//
//
//







#include <stdio.h>
#include "material_coords_raincell.h"
#include "true_vertical_profile.h"

int main(void){
	printf("Programme started.\n");


	Raincell* raincell = create_raincell(1, 0.3, 15000.0, -0.5, 1000.0, 1500.0, 0.0);
	print_raincell(raincell);
	free_raincell(raincell);

	initialise_profile_variables();
	
	printf("Vertical Reflectivity Profile (Top to Bottom)\n");
	printf("=============================================\n\n");

    	for (double h = height_max_echo_top+500; h >= 0; h -= 250.0) {
        	double refl = reflectivity(h);
        	draw_reflectivity_bar(h, refl);
    	}

//internal_geometry_raincell();

printf("Programme finished.\n");
return 0;
}



