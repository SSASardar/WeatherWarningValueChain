#include "vertical_profiles.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


int main() {
    clock_t start, end;
    double cpu_time_used;

    start = clock();

    
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
    return 0;
}

