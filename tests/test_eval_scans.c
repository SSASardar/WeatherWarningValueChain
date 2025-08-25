#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>
#include <float.h>
#include "radars.h"
#include "material_coords_raincell.h"
#include "spatial_coords_raincell.h"
#include "common.h"
#include "processing.h"
#include "vertical_profiles.h"

#define NUM_SCANS 48  // 0..47

typedef struct {
    double mse;
    double mae;
    double bias;
} RainfallStats;

int main() {
    clock_t start = clock();

    RainfallStats stats_array[NUM_SCANS];

    // -------------------------------
    // Step 0: setup VPR structures once
    VPR_params *params = malloc(sizeof(VPR_params));
    init_VPR_params(params);
    fill_VPR_params(params,
        60.0*60, 120.0*60.0, 170.0*60.0, 200.0*60.0, 230.0*60.0,
        10.0, 8000.0, 2000.0,
        50.0, 3.0, 2.0, 2.0,
        3000.0, 100.0, 50.0,
        25.0, 4.0, 3.0,
        1000.0, 4500.0, 750.0,
        0.4, -0.2, -0.15, -0.05,
        35, -3.0, 2.0, 20.0,
        500.0, 150.0, 25.0
    );

    VPR *VPR_strat   = create_and_fill_VPR(params);
    VPR *VPR_conv    = create_VPR();
    VPR *VPR_dummy   = create_VPR();
    VPR *VPR_A_clima = create_VPR();
    VPR *VPR_A_gmd   = create_VPR();
    VPR *VPR_A_d     = create_VPR();

    double t1 = params->t_growth_start;
    double t2 = params->t_decay_end;
    double t3 = params->t_mature_end;

    compute_climatology_VPR(VPR_A_clima, params, 60.0, 70.0*60.0, VPR_dummy);
    compute_average_VPR(VPR_A_gmd, params, t1, t2, 60.0, VPR_dummy);
    compute_average_VPR(VPR_A_d, params, t3, t2, 60.0, VPR_dummy);


    Raincell* raincell = create_raincell(1, 0.5, 10000.0, -0.5, 1000.0, 8000.0, 60.0*60.0, 180.0*60.0, 210.0*60.0, 270.0*60.0);   
Spatial_raincell* s_raincell = create_spatial_raincell(1, -120000.0,80000.0, 6);

    double cart_grid_res = 25;
/*
    // Allocate cart_grids array once (max scan_count will be reused)
    Cart_grid **cart_grids = malloc(100 * sizeof(Cart_grid*));  // adjust 100 if needed

    for (int scan_idx = 0; scan_idx < NUM_SCANS; scan_idx++) {
        char filename[256];
        snprintf(filename, sizeof(filename), "outputs/radar_scan_%04d.txt", scan_idx);
read_radar_scans(filename);  // just call it
// optionally check if scan_count > 0 to ensure success
if (scan_count == 0) {
    fprintf(stderr, "No radar scans loaded from %s\n", filename);
    continue;
}

        printf("Processing %s (scan %d/%d)...\n", filename, scan_idx+1, NUM_SCANS);

        int cg_count = 0;

        // Convert Polar_box → Cart_grid
        for (int i = 0; i < scan_count; i++) {
            Polar_box* p_box = radar_scans[i].box;
            Radar* radar = radar_scans[i].radar;
            double time = radar_scans[i].time;

            update_VPR(VPR_strat, params, time, VPR_conv);
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

            // Fill Cart_grid
            for (int xi = 0; xi < num_x; xi++) {
                for (int yi = 0; yi < num_y; yi++) {
                    int idA = xi * num_y + yi;
                    Point p = {ref_point.x + xi*cart_grid_res, ref_point.y + yi*cart_grid_res};
                    int range_idx, angle_idx;
                    if (getPolarBoxIndex(p, radar->x, radar->y, p_box, &range_idx, &angle_idx)) {
                        int p_grid_idx = range_idx * (int)(p_box->num_angles) + angle_idx;
                        cg->height_grid[idA] = p_box->height_grid[p_grid_idx];

                        if (p_box->grid[p_grid_idx] == 0)
                            cg->grid[idA] = p_box->grid[p_grid_idx];
                        else if (p_box->grid[p_grid_idx] == 1)
                            cg->grid[idA] = get_reflectivity_at_height(VPR_strat, p_box->height_grid[p_grid_idx]);
                        else
                            cg->grid[idA] = get_reflectivity_at_height(VPR_conv, p_box->height_grid[p_grid_idx]);

                        for (int k = 0; k < 3; k++)
                            cg->attenuation_grid[3*idA+k] = p_box->attenuation_grid[3*p_grid_idx+k];
                    } else {
                        cg->grid[idA] = NAN;
                        cg->height_grid[idA] = NAN;
                        for (int k = 0; k < 3; k++) cg->attenuation_grid[3*idA+k] = NAN;
                    }
                }
            }

            cart_grids[cg_count++] = cg;
        }

        // Initialize Vol_scan
        Vol_scan *vol = init_vol_scan(cart_grids, cg_count);
        if (!vol) {
            fprintf(stderr, "Failed to init volume scan\n");
            continue;
        }
        for (int i = 0; i < cg_count; i++)
            add_cart_grid_to_volscan(vol, cart_grids[i], i);

*/

for (int scan_idx = 0; scan_idx < NUM_SCANS; scan_idx++) {
    char filename[256];
    snprintf(filename, sizeof(filename), "outputs/radar_scan_%04d.txt", scan_idx);

    read_radar_scans(filename);
    if (scan_count == 0) {
        fprintf(stderr, "No radar scans loaded from %s\n", filename);
        continue;
    }

    printf("Processing %s (scan %d/%d)...\n", filename, scan_idx+1, NUM_SCANS);

    // Allocate cart_grids for this scan
    Cart_grid **cart_grids = malloc(scan_count * sizeof(Cart_grid*));
    if (!cart_grids) { fprintf(stderr, "Malloc failed\n"); exit(1); }

    int cg_count = 0;

    for (int i = 0; i < scan_count; i++) {
        Polar_box* p_box = radar_scans[i].box;
        Radar* radar = radar_scans[i].radar;
        double time = radar_scans[i].time;

        update_VPR(VPR_strat, params, time, VPR_conv);

        Bounding_box* bbox = bounding_box_from_textfile(p_box, radar);

        // compute safe num_x and num_y
        int num_x = (int)(ceil((bbox->bottomRight.x - bbox->bottomLeft.x)/cart_grid_res));
        int num_y = (int)(ceil((bbox->topLeft.y - bbox->bottomLeft.y)/cart_grid_res));
        if (num_x <= 0) num_x = 1;
        if (num_y <= 0) num_y = 1;

        Point ref_point = {
            ceil(bbox->bottomLeft.x / cart_grid_res) * cart_grid_res,
            ceil(bbox->bottomLeft.y / cart_grid_res) * cart_grid_res
        };

        Cart_grid *cg = Cart_grid_init(cart_grid_res, num_x, num_y, ref_point);
        if (!cg) continue;

        // Fill Cart_grid
        for (int xi = 0; xi < num_x; xi++) {
            for (int yi = 0; yi < num_y; yi++) {
                int idA = xi * num_y + yi;
                Point p = {ref_point.x + xi*cart_grid_res, ref_point.y + yi*cart_grid_res};
                int range_idx, angle_idx;

                if (getPolarBoxIndex(p, radar->x, radar->y, p_box, &range_idx, &angle_idx)) {
                    int p_grid_idx = range_idx * (int)p_box->num_angles + angle_idx;
                    cg->height_grid[idA] = p_box->height_grid[p_grid_idx];

                    if (p_box->grid[p_grid_idx] == 0)
                        cg->grid[idA] = p_box->grid[p_grid_idx];
                    else if (p_box->grid[p_grid_idx] == 1)
                        cg->grid[idA] = get_reflectivity_at_height(VPR_strat, p_box->height_grid[p_grid_idx]);
                    else
                        cg->grid[idA] = get_reflectivity_at_height(VPR_conv, p_box->height_grid[p_grid_idx]);

                    for (int k = 0; k < 3; k++)
                        cg->attenuation_grid[3*idA+k] = p_box->attenuation_grid[3*p_grid_idx+k];
                } else {
                    cg->grid[idA] = NAN;
                    cg->height_grid[idA] = NAN;
                    for (int k = 0; k < 3; k++) cg->attenuation_grid[3*idA+k] = NAN;
                }
            }
        }

        cart_grids[cg_count++] = cg;
    }

    // Init Vol_scan
    Vol_scan *vol = init_vol_scan(cart_grids, cg_count);
    for (int i = 0; i < cg_count; i++)
        add_cart_grid_to_volscan(vol, cart_grids[i], i);



compute_display_grid_max(vol,10.0);

 double true_time = radar_scans[scan_count-1].time + ( radar_scans[scan_count-1].time -  radar_scans[scan_count-2].time  );
 update_VPR(VPR_strat, params, true_time, VPR_conv);
 // initialize your cell, e.g. my_cell.initial_x, my_cell.dx, etc.

Point* raincell_pos = get_position_raincell(true_time, s_raincell);
if (!raincell_pos) {
    fprintf(stderr, "Failed to get raincell position\n");
    exit(EXIT_FAILURE);
}
//printf("Raincell position: x=%f, y=%f\n", raincell_pos->x, raincell_pos->y);


if (fill_refl_ALA_grid(vol, raincell_pos, raincell, VPR_strat, VPR_conv) != 0) {
    fprintf(stderr, "Failed to fill Refl_ALA grid\n");
}




        // Compute rainfall statistics
        double mse, mae, bias;
        if (compute_rainfall_statistics(vol,10.0, &mse, &mae, &bias) == 0) {
            stats_array[scan_idx].mse = mse;
            stats_array[scan_idx].mae = mae;
            stats_array[scan_idx].bias = bias;
        } else {
            stats_array[scan_idx].mse = stats_array[scan_idx].mae = stats_array[scan_idx].bias = NAN;
        }

        // Free Cart_grids for next scan
        for (int i = 0; i < cg_count; i++)
            free_cart_grid(cart_grids[i]);
        free_vol_scan(vol);
    }

    // -------------------------------
    // Output statistics to a CSV file
    FILE *fp = fopen("outputs/stats.txt", "w");
    if (fp) {
        fprintf(fp, "Scan,MSE,MAE,Bias\n");
        for (int i = 0; i < NUM_SCANS; i++) {
            fprintf(fp, "%d %.5f %.5f %.5f\n", i, stats_array[i].mse, stats_array[i].mae, stats_array[i].bias);
        }
        fclose(fp);
        printf("Statistics saved to outputs/stats.csv\n");
    } else {
        fprintf(stderr, "Failed to open outputs/stats.csv for writing\n");
    }

    free(params);
//    free(cart_grids);

    clock_t end = clock();
    printf("Total time: %f seconds\n", (double)(end - start)/CLOCKS_PER_SEC);

    return 0;
}

