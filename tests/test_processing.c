#include <stdio.h>                                   
#include <math.h>
#include <stdlib.h>
#include "radars.h"
#include "material_coords_raincell.h"
#include "spatial_coords_raincell.h"
#include "common.h"
#include <time.h>
#include "processing.h"
#include<stdbool.h>


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
	print_polar_box(p_box);
	printf("Scan %d: First grid value: %.2f\n", 
               radar_scans[i].scan_index, p_box->grid[0]);
 
 	// create bounding box for the polar box
	
	Bounding_box* bbox = bounding_box_from_textfile(p_box, radar);	
	print_bounding_box(bbox); 

	// Initialising the cartesian grid structure:	
	num_x = (int)fabs(ceil((bbox->bottomLeft.x)/cart_grid_res) - floor((bbox->bottomRight.x)/cart_grid_res));
	num_y = (int)fabs(ceil((bbox->bottomLeft.y)/cart_grid_res) - floor((bbox->topLeft.y)/cart_grid_res));
	Point ref_point;
	ref_point.x = ceil((bbox->bottomLeft.x)/cart_grid_res)*cart_grid_res;
	ref_point.y = ceil((bbox->bottomLeft.y)/cart_grid_res)*cart_grid_res;
	printf("ref_point = (%.2lf, %.2lf)\n\n", ref_point.x, ref_point.y);
 	Cart_grid* cg = Cart_grid_init(cart_grid_res, num_x, num_y, ref_point);
	
	printf("there are %d num_x and %d num_y\n", cg->num_x, cg->num_y);
	printf("There are %d elements in the cartesian grid \n",cg->num_elements);
    
    	Point p;
	double c_x = radar->x;
	double c_y = radar->y;

	int range_idx, angle_idx;
	int z = 0;
	for(int xi = 0; xi<num_x;xi++){
		p.x = ref_point.x + xi*cart_grid_res;		
		for(int yi = 0; yi<num_y;yi++){
			p.y = ref_point.y + yi*cart_grid_res;		
//			if(z%10000 == 0){printf("the point tested is (%.2lf, %.2lf)\n and has result %d\n", p.x,p.y,getPolarBoxIndex(p, c_x, c_y, p_box, &range_idx, &angle_idx));}
			if (getPolarBoxIndex(p, c_x, c_y, p_box, &range_idx, &angle_idx)) {
			    int p_grid_idx = range_idx * (int)(p_box->num_angles) + angle_idx;
			    cg->grid[xi*num_y+yi] = p_box->grid[p_grid_idx];
			    cg->height_grid[xi*num_y+yi] = p_box->height_grid[p_grid_idx];

			} else {
			    cg->grid[xi*num_y+yi] = -1.0;// Handle point outside polar box
			    cg->height_grid[xi*num_y+yi] = -1.0;// Handle point outside polar box
			}
		z = z+1;
		}
	}
   
   writeCartGridToFile(cg,i);
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





