#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "radars.h"
#include "material_coords_raincell.h"
#include "spatial_coords_raincell.h"
#include "common.h"

int main() {
    // Create radar list
    Radar* radar1 = create_radar(1, "C", "RHI", 0.0, 0.0, 100.0, 250000.0, 250.0, 1.0);
    Radar* radar2 = create_radar(2, "X", "RHI", -50000.0, 50000.0, 60.0, 50000.0, 50.0, 1.0);

const Radar* radars[] = { radar1, radar2 };
    int num_radars = sizeof(radars) / sizeof(radars[0]);

    // Create raincell and spatial_raincell
    Raincell* raincell = create_raincell(1, 0.7, 3000.0, -0.5, 1000.0, 8000.0, 60.0*60.0, 180.0*60.0, 210.0*60.0, 270.0*60.0);   
    
    Spatial_raincell* s_raincell = create_spatial_raincell(1, -120000.0,80000.0, 6);
	
    double time = 210.00*60;
    Bounding_box* true_box = create_BoundingBox_for_s_raincell(s_raincell, time, raincell);


    // Create polar box
    Polar_box* box = create_polar_box(time, s_raincell, radar2, raincell);

    Bounding_box* bounded_polar_box = create_bounding_box_for_polar_box(box, radars, num_radars);

    // Find radar associated with polar box
    const  Radar* found_radar = find_radar_by_id(box, radars, num_radars);
    if (found_radar) {
        printf("Radar found: ID = %d, Frequency = %s, Max Range = %.2f\n",
               get_radar_id(found_radar), get_frequency(found_radar), get_max_range_radar(found_radar));
    } else {
        printf("Radar not found.\n");
    }



    print_polar_grid(box, radars, num_radars);

	Point* centre_s_raincell = get_position_raincell(time,s_raincell);
	printf("Description of the spatial raincell:\n \n (%.0lf, %.0lf)    ±    %.0lf\n\n", centre_s_raincell->x, centre_s_raincell->y, raincell_get_radius_stratiform(raincell));

	printf("Does the polar bounding box include the true bounding box? (0 is no, 1 is yes)\n");
	printf("%d\n", does_box1_include_box2(bounded_polar_box, true_box));
	printf("true box:\n");
	print_bounding_box(true_box);
	printf("bounded polar box:\n");
	print_bounding_box(bounded_polar_box);


	printf("\n\n\n now we test this for a bunch of times\n\n");

	







    // Cleanup
       free_spatial_raincell(s_raincell);
         free_raincell(raincell);

    
    free(radar1);
    free(radar2);
    free(box);

    return 0;










}
