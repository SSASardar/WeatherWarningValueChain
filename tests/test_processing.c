#include <stdio.h>                                   
#include <math.h>
#include <stdlib.h>
#include "radars.h"
#include "material_coords_raincell.h"
#include "spatial_coords_raincell.h"
#include "common.h"
#include <time.h>
#include "processing.h"

int main() {
	clock_t start = clock();
    
	read_radar_scans("outputs/radar_scans.txt");

    printf("Loaded %d radar scans:\n", scan_count);
    for (int i = 0; i < scan_count; ++i) {
        RadarScan* s = &radar_scans[i];
        printf("Scan %d: Radar ID %d, Grid[0]=%.2f\n",
               s->scan_index, s->radar->id,
               s->box->grid ? s->box->grid[65] : -1.0);
    }

// Defining the cartesian grid resolution:

    double cart_grid_res = 12.5;
	int num_x, num_y;
int target_radar_id = 1;
for (int i = 0; i < scan_count; ++i) {
    if (radar_scans[i].radar->id == target_radar_id) {
        Polar_box* p_box = radar_scans[i].box;
        Radar* radar = radar_scans[i].radar;

	printf("Scan %d: First grid value: %.2f\n", 
               radar_scans[i].scan_index, p_box->grid[0]);
 
 	// create bounding box for the polar box
	
	Bounding_box* bbox = bounding_box_from_textfile(p_box, radar);	
	print_bounding_box(bbox); 

	// Initialising the cartesian grid structure:	
	num_x = (int)fabs(ceil((bbox->bottomLeft.x)/cart_grid_res) - floor((bbox->bottomRight.x)/cart_grid_res));
	num_y = (int)fabs(ceil((bbox->bottomLeft.y)/cart_grid_res) - floor((bbox->topLeft.y)/cart_grid_res));
	Point ref_point;
	ref_point.x = ceil((bbox->bottomLeft.x)/cart_grid_res);
	ref_point.y = ceil((bbox->bottomLeft.y)/cart_grid_res);
 	Cart_grid* cg = Cart_grid_init(cart_grid_res, num_x, num_y, ref_point);

	printf("there are %d num_x and %d num_y\n", cg->num_x, cg->num_y);
	printf("There are %d elements in the cartesian grid \n",cg->num_elements);
    }
}

    clock_t end = clock();

    double time_taken = (double)(end - start) / CLOCKS_PER_SEC;

    printf("Time taken: %f seconds\n", time_taken);


    // Free memory
    for (int i = 0; i < radar_count; ++i)
        free(radar_list[i]);
    for (int i = 0; i < scan_count; ++i)
        free(radar_scans[i].box->grid), free(radar_scans[i].box);

    return 0;
}





