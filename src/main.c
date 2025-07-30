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
#include <plplot/plplot.h>
#include "plot.h"


int main(void){
	printf("Programme started.\n'\n");


	Raincell* raincell = create_raincell(1, 0.4, 5000.0, -0.5, 1000.0, 8000.0, 60.0*60.0, 180.0*60.0, 210.0*60.0, 270.0*60.0);
	print_raincell(raincell);


	Spatial_raincell* s_raincell = create_spatial_raincell(1, -120000.0,80000.0, 6);

	print_path_spatial_raincell(s_raincell);

/*
 // TESTING LOCATION AND HEIGHT OF THE SPATIAL RAINCELL
 
	printf("\n+\n+\n+\n   START of simulation: I am in c-band range, not in X-band range   \n+\n+\n+\n");
	print_spatial_raincell(s_raincell, 60*0, raincell);

	printf("\n+\n+\n+\n   Start of growth, not in X-band FOV yet (at y = 80km)   \n+\n+\n+\n");
	print_spatial_raincell(s_raincell, 60*60, raincell);

	printf("\n+\n+\n+\n   Cell centre enters X-band FOV and is growing in height   \n+\n+\n+\n");
	print_spatial_raincell(s_raincell, 60*90, raincell);

	printf("\n+\n+\n+\n   growth stops, max height reached   \n+\n+\n+\n");
	print_spatial_raincell(s_raincell, 60*180, raincell);

	printf("\n+\n+\n+\n   decay starts, height decreases   \n+\n+\n+\n");
	print_spatial_raincell(s_raincell, 60*210, raincell);
	
	printf("\n+\n+\n+\n   max height is decaying   \n+\n+\n+\n");
	print_spatial_raincell(s_raincell, 60*240, raincell);
	
	printf("\n+\n+\n+\n   core is rained out   \n+\n+\n+\n");
	print_spatial_raincell(s_raincell, 60*270, raincell);

	printf("\n+\n+\n+\n   Cell leaves FOV of X-band at y = 80km   \n+\n+\n+\n");
	print_spatial_raincell(s_raincell, 60*330, raincell);

	printf("\n+\n+\n+\n   END of simulation: I am in C-band range, not in X-band range   \n+\n+\n+\n");
	print_spatial_raincell(s_raincell, 60*360, raincell);
*/









	free_spatial_raincell(s_raincell);
	
	free_raincell(raincell);

/* //VPR .... there is a mistake.... you initialise the variables, but you can also just do that when you create VPRs.... 	
	initialise_profile_variables();
	printf("Vertical Reflectivity Profile (Top to Bottom)\n");
	printf("=============================================\n\n");

    	for (double h = height_max_echo_top+500; h >= 0; h -= 250.0) {
        	double refl = reflectivity(h);
        	draw_reflectivity_bar(h, refl);
    	}

*/



    setup_plot("output.ps");  // Sets up PDF output

    plot_example();            // Actual plotting

    finalize_plot();           // Closes the PDF cleanly









printf("\nProgramme finished.\n\n");
return 0;
}



