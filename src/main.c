// this is the main.c file
//
//
//
//







#include <stdio.h>
#include <math.h>
#include "material_coords_raincell.h"
#include "true_vertical_profile.h"
#include "spatial_coords_raincell.h"


int main(void){
	printf("Programme started.\n");


	Raincell* raincell = create_raincell(1, 0.3, 15000.0, -0.5, 1000.0, 1500.0, 0.0);
	print_raincell(raincell);


	Spatial_raincell* s_raincell = create_spatial_raincell(1, -265000.0,85000.0, 2.9);


	printf("\n+\n+\n+\n   Start of simulation, not in X-band or C-band  FOV yet  \n+\n+\n+\n");
	print_spatial_raincell(s_raincell, 0, raincell);
	printf("\n+\n+\n+\n   Start of growth, not in X-band FOV yet   \n+\n+\n+\n");
	print_spatial_raincell(s_raincell, 60*60, raincell);
	printf("\n+\n+\n+\n   Cell enters X-band FOV   \n+\n+\n+\n");
	print_spatial_raincell(s_raincell, 60*75, raincell);
	printf("\n+\n+\n+\n   growth stops, max height reached   \n+\n+\n+\n");
	print_spatial_raincell(s_raincell, 60*90, raincell);
	printf("\n+\n+\n+\n   Cell leaves FOV of X-band radar   \n+\n+\n+\n");
	print_spatial_raincell(s_raincell, 60*125, raincell);
	printf("\n+\n+\n+\n   Emd of relevant simulation   \n+\n+\n+\n");
	print_spatial_raincell(s_raincell, 60*150, raincell);



	free_spatial_raincell(s_raincell);
	
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



