#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "radars.h"
#include "material_coords_raincell.h"
#include "spatial_coords_raincell.h"
#include "common.h"
#include <time.h>
#include "processing.h"
#include <stdbool.h>
#include <float.h>
#include "vertical_profiles.h"

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

    // -------------------------------
    // Define Cartesian grid resolution
    double cart_grid_res = 12.5;

    // Step 1: allocate space for Cart_grids
    Cart_grid *cart_grids = malloc(scan_count * sizeof(Cart_grid));
    if (!cart_grids) {
        fprintf(stderr, "Failed to allocate cart_grids\n");
        return 1;
    }

    int cg_count = 0;

    // Step 2: convert Polar_box → Cart_grid
    for (int i = 0; i < scan_count; ++i) {
        Polar_box* p_box = radar_scans[i].box;
        Radar* radar     = radar_scans[i].radar;

        // Build bounding box
        Bounding_box* bbox = bounding_box_from_textfile(p_box, radar);

        int num_x = (int)fabs(ceil((bbox->bottomLeft.x)/cart_grid_res) 
                              - floor((bbox->bottomRight.x)/cart_grid_res));
        int num_y = (int)fabs(ceil((bbox->bottomLeft.y)/cart_grid_res) 
                              - floor((bbox->topLeft.y)/cart_grid_res));

        Point ref_point;
        ref_point.x = ceil((bbox->bottomLeft.x)/cart_grid_res) * cart_grid_res;
        ref_point.y = ceil((bbox->bottomLeft.y)/cart_grid_res) * cart_grid_res;

        Cart_grid *cg = Cart_grid_init(cart_grid_res, num_x, num_y, ref_point);
        if (!cg) continue;

        Point p;
        double c_x = radar->x;
        double c_y = radar->y;
        int range_idx, angle_idx;

        for (int xi = 0; xi < num_x; xi++) {
            p.x = ref_point.x + xi * cart_grid_res;
            for (int yi = 0; yi < num_y; yi++) {
                int idA = xi * num_y + yi;
                p.y = ref_point.y + yi * cart_grid_res;

                if (getPolarBoxIndex(p, c_x, c_y, p_box, &range_idx, &angle_idx)) {
                    int p_grid_idx = range_idx * (int)(p_box->num_angles) + angle_idx;
                    cg->grid[idA]         = p_box->grid[p_grid_idx];
                    cg->height_grid[idA]  = p_box->height_grid[p_grid_idx];
                    cg->attenuation_grid[3*idA+0] = p_box->attenuation_grid[3*p_grid_idx+0];
                    cg->attenuation_grid[3*idA+1] = p_box->attenuation_grid[3*p_grid_idx+1];
                    cg->attenuation_grid[3*idA+2] = p_box->attenuation_grid[3*p_grid_idx+2];
                } else {
                    cg->grid[idA]         = NAN;
                    cg->height_grid[idA]  = NAN;
                    cg->attenuation_grid[3*idA+0] = NAN;
                    cg->attenuation_grid[3*idA+1] = NAN;
                    cg->attenuation_grid[3*idA+2] = NAN;
                }
            }
        }

        cart_grids[cg_count++] = *cg;
    }

    // Step 3: initialize Vol_scan (once you have all Cart_grids)
    Vol_scan *vol = init_vol_scan(cart_grids, cg_count);
    if (!vol) {
        fprintf(stderr, "Failed to init volume scan\n");
        return 1;
    }

    // Step 4: add each Cart_grid into the Vol_scan
    for (int i = 0; i < cg_count; i++) {
        add_cart_grid_to_volscan(vol, &cart_grids[i], i);
    }

    printf("Volume scan created: %d PPIs, %d x %d grid, %d elements\n",
           vol->num_PPIs, vol->num_x, vol->num_y, vol->num_elements);

    // Example: inspect one point
    int test_x = 1500, test_y = 1000, test_ppi = 10;
    int idx = test_ppi * vol->num_elements + test_y * vol->num_x + test_x;
    if (idx < vol->num_elements * vol->num_PPIs) {
        printf("Sample VPR point: refl=%.2f, h=%.2f\n",
               vol->grid[idx].reflectivity,
               vol->grid[idx].height);
    }


    write_vol_scan_ppi_to_file(vol, 10, "outputs/ppi10.txt");


    // Step 5: cleanup
    free_vol_scan(vol);
    free(cart_grids);

    clock_t end = clock();
    printf("Time taken: %f seconds\n", (double)(end - start) / CLOCKS_PER_SEC);

    return 0;
}

