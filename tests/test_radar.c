#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "radars.h"
#include "material_coords_raincell.h"
#include "spatial_coords_raincell.h"
#include "common.h"

int main() {
    // Create radar list
    Radar* radar1 = create_radar(1, "C", "PPI", 0.0, 0.0, 100.0, 250000.0, 250.0, 1.0);
    Radar* radar2 = create_radar(2, "X", "PPI", -50000.0, 50000.0, 60.0, 50000.0, 50.0, 1.0);

const Radar* radars[] = { radar1, radar2 };
    int num_radars = sizeof(radars) / sizeof(radars[0]);

    // Create raincell and spatial_raincell
    Raincell* raincell = create_raincell(1, 0.5, 10000.0, -0.5, 1000.0, 8000.0, 60.0*60.0, 180.0*60.0, 210.0*60.0, 270.0*60.0);   
    
    Spatial_raincell* s_raincell = create_spatial_raincell(1, -120000.0,80000.0, 6);
	
	Radar* radar_test = radar1;

    //double time = 210.00*60;
    double time;
    Polar_box* box = init_polar_box();
    
    for (int ti = 0; ti<15;ti++){
	if(ti == 4){radar_test = radar2;}
	if(ti == 10){radar_test = radar1;}
    	    time = (0.0+(double)ti*30.0)*60.0;
      	Bounding_box* true_box = create_BoundingBox_for_s_raincell(s_raincell, time, raincell);

/*
    // Create polar box
    Polar_box* box = create_polar_box(time, s_raincell, radar2, raincell);
*/

	// Fill polar box

	    if (fill_polar_box(box, time, s_raincell, radar_test, raincell) != 0) {
        printf("Failed to update polar box for iteration %d\n", ti);
        // handle error if needed
    }


    Bounding_box* bounded_polar_box = create_bounding_box_for_polar_box(box, radars, num_radars);

print_bounding_box(bounded_polar_box);

    // Find radar associated with polar box
    const  Radar* found_radar = find_radar_by_id(box, radars, num_radars);
    if (found_radar) {
        printf("Radar found: ID = %d, Frequency = %s, Max Range = %.2f\n",
               get_radar_id(found_radar), get_frequency(found_radar), get_max_range_radar(found_radar));
    } else {
        printf("Radar not found.\n");
    }


/*
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

	
*/


printf("Does the height function work properly?\n\n");
printf("you need to check still... haha\n\n");

/*
printf("Does the core or no core function work properly?\n\n");
double r1 = (get_min_range_gate(box)+0.5)*get_range_res_polar_box(box);
double a1 = get_min_angle(box);
double a3 = get_max_angle(box);
double a2 = a3 - 12;
double r2 = (get_min_range_gate(box)+75+0.5)*get_range_res_polar_box(box);
struct Point* pos_r2 = get_position_radar(radar2);

double height_1 = calculate_height_of_beam_at_range(r1, 0.0, 6378000.0, get_height_of_radar(radar2));
int where_in_cell = sample_from_relative_location_in_raincell(r1,a1, pos_r2, get_position_raincell(time, s_raincell), raincell);
*/


/*
printf("For (r,theta) = (%.0lf, %.0lf),\n point (x,y) = (%.0lf, %.0lf) is sampled at:\n 		height = %.0lf\n 	with\n		 	%d rain\n 	no rain (0), stratiform rain (1), or in developing core rain (2)\n\n", r1,a1,pos_r2->x+r1*cos(a1*DEG2RAD), pos_r2->y+r1*sin(a1*DEG2RAD), height_1, where_in_cell);

height_1 = calculate_height_of_beam_at_range(r2, 0.0, 6378000.0, get_height_of_radar(radar2));
where_in_cell = sample_from_relative_location_in_raincell(r2,a2, pos_r2, get_position_raincell(time, s_raincell), raincell);
 
printf("For (r,theta) = (%.0lf, %.0lf),\n point (x,y) = (%.0lf, %.0lf) is sampled at:\n                 height = %.0lf\n        with\n                  %d rain\n       no rain (0), stratiform rain (1), or in developing core rain (2)\n\n", r2,a2,pos_r2->x+r2*cos(a2*DEG2RAD), pos_r2->y+r2*sin(a2*DEG2RAD), height_1, where_in_cell);

*/
printf("\n\nif you see a number of 0's, 1's and 2's you know the test works.\n\n");

/*for (int ri = 0; ri < (int)get_num_ranges(box);ri++){
	r1 = (get_min_range_gate(box)+ri+0.5)*get_range_res_polar_box(box);
//printf("==================\n\n Checking at range %.0lf on all angles:\n",r1);

for (int ai = 0; ai < 10* (int)get_num_angles(box); ai++){
	a1 = get_min_angle(box)+(ai+0.5)*0.1*get_angular_res_polar_box(box);
	printf("%d ", sample_from_relative_location_in_raincell(r1,a1, pos_r2, get_position_raincell(time, s_raincell), raincell));
}
	printf("\n");
}
*/
/*
FILE *fp = fopen("outputs/test_radar_view.txt", "w");
if (!fp) {
    perror("Failed to open output file");
    exit(1);
}
fprintf(fp, "# min_range=%.2f range_res=%.2f min_angle=%.2f angular_res=%.4f max_range=%.2f num_ranges=%d num_angles=%d\n",
        get_min_range_gate(box),
        get_range_res_polar_box(box),
        get_min_angle(box),
        get_angular_res_polar_box(box),  // interpolated step
	get_max_range_radar(radar2),
        (int)get_num_ranges(box),
        (int)get_num_angles(box));

for (int ri = 0; ri < (int)get_num_ranges(box); ri++) {
    r1 = (get_min_range_gate(box) + ri + 0.5) * get_range_res_polar_box(box);
    for (int ai = 0; ai < (int)get_num_angles(box); ai++) {
        a1 = (get_max_angle(box) - (ai + 0.5)) * get_angular_res_polar_box(box);
        fprintf(fp, "%d ", sample_from_relative_location_in_raincell(r1, a1, pos_r2, get_position_raincell(time, s_raincell), raincell));
    }
    fprintf(fp, "\n");
}
fclose(fp);

*/

fill_polar_box_grid(box, radar_test, s_raincell, raincell, time);

save_polar_box_grid_to_file(box, radar_test, ti, "outputs/radar_scans.txt");
}
printf("===================\n\n");
// Cleanup
       free_spatial_raincell(s_raincell);
         free_raincell(raincell);

    
    free(radar1);
    free(radar2);
    free(box);

    return 0;










}
