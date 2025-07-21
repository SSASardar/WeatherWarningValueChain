#include "true_vertical_profile.h"
#include <stdio.h>
#include "math.h"
#include "material_coords_raincell.h"
#include "common.h"


// local variables
double height_base = 1000.0;
double height_max_echo_top = 8000.0;
double height_brightband = 3500.0;
double width_brightband = 1000.0;
double height_brightband_l;
//height_brightband_l = height_brightband-width_brightband;
double height_brightband_u;
//height_brightband_u = height_brightband+width_brightband;
double reflectivity_base = 40.0;
double reflectivity_max_echo_top = 10.0;
double reflectivity_brightband = 60.0;
double reflectivity_absdiff_brightband = 15.0;
double reflectivity_brightband_l;
//reflectivity_brightband_l = height_brightband-reflectivity_absdiff_brightband*2/3;
double reflectivity_brightband_u;
//reflectivity_brightband_u = height_brightband-reflectivity_absdiff_brightband/3;
double gradient_base_to_bottom_T0;
//gradient_base_to_bottom_T0 = (height_brightband_l - height_base) / (reflectivity_brightband_l - reflectivity_base);
double gradient_bottom_T0_to_T0;
//gradient_bottom_T0_to_T0 = (height_brightband - height_brightband_l) / (reflectivity_brightband - reflectivity_brightband_l);
double gradient_T0_to_top_T0;
//gradient_T0_to_top_T0 = (height_brightband_u - height_brightband) / (reflectivity_brightband_u - reflectivity_brightband);
double gradient_top_T0_to_max_echo_top;
//gradient_top_T0_to_max_echo_top = (height_max_echo_top - height_brightband_u) / (reflectivity_max_echo_top - reflectivity_brightband_u);



// function retrieving the reflectivity at a given height for the vertical profile of reflectivity. 

double reflectivity(double height){
	if (height<height_base || height>height_max_echo_top){
		return 0.0;
	}
	else if (height_base <= height && height < height_brightband_l) {
		return (1/gradient_base_to_bottom_T0)*(height-height_base) + reflectivity_base;
	}
	else if (height_brightband_l <= height && height < height_brightband) {
                return (1/gradient_bottom_T0_to_T0)*(height-height_brightband_l) + reflectivity_brightband_l;
        }
	else if (height_brightband <= height && height < height_brightband_u) {
                return (1/gradient_T0_to_top_T0)*(height-height_brightband) + reflectivity_brightband;
        }
	else if (height_brightband_u <= height && height < height_max_echo_top) {
                return (1/gradient_top_T0_to_max_echo_top)*(height-height_brightband_u) + reflectivity_brightband_u;
        }

	return 0.0;
}

void initialise_profile_variables() {
    height_brightband_l = height_brightband - width_brightband;
    height_brightband_u = height_brightband + width_brightband;

//    reflectivity_brightband_l = reflectivity_brightband - reflectivity_absdiff_brightband * 1.0 / 3.0;
    reflectivity_brightband_l = reflectivity_base;
    reflectivity_brightband_u = reflectivity_brightband - reflectivity_absdiff_brightband * 3.0 / 3.0;

    gradient_base_to_bottom_T0 = (height_brightband_l - height_base) /
                                 (reflectivity_brightband_l - reflectivity_base);

    gradient_bottom_T0_to_T0 = (height_brightband - height_brightband_l) /
                               (reflectivity_brightband - reflectivity_brightband_l);

    gradient_T0_to_top_T0 = (height_brightband_u - height_brightband) /
                            (reflectivity_brightband_u - reflectivity_brightband);

    gradient_top_T0_to_max_echo_top = (height_max_echo_top - height_brightband_u) /
                                      (reflectivity_max_echo_top - reflectivity_brightband_u);

}




// Print a horizontal bar of reflectivity
void draw_reflectivity_bar(double height, double reflectivity) {
    int num_stars = (int)(reflectivity / 2.0); // adjust scale
    printf("%5.0f m | ", height);
    for (int i = 0; i < num_stars; ++i) {
        putchar('*');
    }
    printf(" %.1f dBZ\n", reflectivity);
}

