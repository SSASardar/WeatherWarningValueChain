#include "vertical_profiles.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


int main() {
    clock_t start, end;
    double cpu_time_used;

    start = clock();

    /* // initial testing block 1
	VPR_params *params = malloc(sizeof(VPR_params));

    init_VPR_params(params);

    fill_VPR_params(params,
//        60.0*60, 180.0*60.0, 210.0*60.0, 240.0*60.0, 270.0*60.0,   // times
        60.0*60, 120.0*60.0, 180.0*60.0, 210.0*60.0, 240.0*60.0,   // times
        10.0, 8000.0, 2000.0,        // Echo top
        50.0, 3.0, 2.0, 1.5,       // Bright band Z
        3000.0, 1000.0, 500.0,       // Bright band heights
        20.0, 3.0, 2.0,            // Width Z
        1000.0, -500.0, 250.0,             // Width H
        0.4,                       // Ratio
        35, -3.0, 2.0, 12.0,       // Cell base Z
        500.0, 150.0, 25.0          // Cell base heights
    );

const char* stages[] = {
    "growth start",
    "growth middle",
    "mature start",
    "mature mid",
    "mature end",
    "decay mid",
    "decay end"
};


    printf("t_growth_start = %.2f\n", params->t_growth_start);
    printf("Z_et_0 = %.2f\n", params->Z_et_0);

        // Create and fill the two VPR structures
    VPR *VPR_strat = create_and_fill_VPR(params);
    VPR *VPR_conv  = create_and_fill_VPR(params);
	

    printf("strat initial VPR: \n");
    print_VPR_points(VPR_strat);
    printf("\n\n");

    printf("conv initial VPR: \n");
	print_VPR_points(VPR_conv);


printf("\n\n");

	
	
	double height_t1 = 4000;
	printf("starting to test the parametrisation work as expected in the different phases.\n +++++++++++++++\n\n+++++++++++++++\n");
for(int i = 0; i<7;i++){
	double time  = (60.0+30.0*i)*60.0;
printf("conv at %.0f minutes (%s): \n", 60.0 + 30.0 * i, stages[i]);
    update_VPR(VPR_strat, params, time, VPR_conv);
	print_VPR_points(VPR_conv);
	printf("reflectivity at %.2f km height is %.2f\n\n", height_t1, get_reflectivity_at_height(VPR_conv, height_t1));
	printf("\n\n");
}

    end = clock();

    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

    printf("Elapsed time: %f seconds\n", cpu_time_used);

    free(params);
    */ // end of block 1




// start of block 2, 


       VPR_params *params = malloc(sizeof(VPR_params));
    init_VPR_params(params);

    fill_VPR_params(params,
        100.0*60, 130.0*60.0, 150.0*60.0, 170.0*60.0, 200.0*60.0,
        10.0, 8000.0, 2000.0,        // Echo top
        50.0, 3.0, 2.0, 2.0,         // Bright band Z
        3000.0, 100.0, 50.0,       // Bright band heights
        25.0, 4.0, 3.0,              // Width Z
        1000.0, 4500.0, 750.0,       // Width H
        0.4,-0.2, -0.1, -0.05,           // Ratio
        35, -3.0, 2.0, 20.0,         // Cell base Z
        500.0, 150.0, 25.0           // Cell base heights
    );

    VPR *VPR_strat = create_and_fill_VPR(params);
    VPR *VPR_conv  = create_and_fill_VPR(params);

    double height_min = 0.0;
    double height_max = 10000.0;
    double height_step = 50.0;
    int n_heights = (int)((height_max - height_min) / height_step) + 1;

    double time_min = 0.0;
    double time_max = 270.0 * 60.0; // seconds
    double time_step = 5.0 * 60.0;  // seconds
    int n_times = (int)((time_max - time_min) / time_step) + 1;

    FILE *f = fopen("outputs/reflectivity_profile.txt", "w");
    if (!f) {
        perror("Failed to open file");
        free(params);
        return 1;
    }

    // Header
    fprintf(f, "Height");
    for (int t = 0; t < n_times; t++) {
        double time_minutes = (time_min + t * time_step) / 60.0;
        fprintf(f, "\tT%.0f", time_minutes);
    }
    fprintf(f, "\n");

    // Heights and reflectivities
    for (int h = 0; h < n_heights; h++) {
        double height = height_min + h * height_step;
        fprintf(f, "%.2f", height);

        for (int t = 0; t < n_times; t++) {
            double time_seconds = time_min + t * time_step;
            update_VPR(VPR_strat, params, time_seconds, VPR_conv);
            double refl = get_reflectivity_at_height(VPR_conv, height);
            fprintf(f, "\t%.2f", refl);
        }
        fprintf(f, "\n");
    }

    fclose(f);

    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Elapsed time: %f seconds\n", cpu_time_used);

    free(params);






    return 0;
}

