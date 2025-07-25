#include <stdio.h>
#include <math.h>
#include "material_coords_raincell.h"
//#include "true_vertical_profile.h"
//#include "spatial_coords_raincell.h"

int main(void) {
    printf("Programme started.\n");

    // Create a material raincell
    Raincell* raincell = create_raincell(
        1,          // 
        0.3,        // 
        15000.0,    // 
        -0.5,       // 
        1000.0,     // 
        8000.0,     // 
        60.0*60.0,  // 
        180.0*60.0, // 
        210.0*60.0, // 
        270.0*60.0  // 
    );
    
print_raincell(raincell);



printf("over time, the top-core height changes as follows.\n");
// TESING THE HEIGHTS OF THE MATERIAL RAINCELL OVER TIME.
for (int i = 0; i < 75; i++){
	double time1 = (double) i*5.0;
	printf("%.2lf	_	_	%.2lf\n", time1 , raincell_get_top_height_core(raincell, time1*60));
}

	printf("Freeing memory:\n");
	free_raincell(raincell);
    printf("Programme finished.\n");
    return 0;
}
