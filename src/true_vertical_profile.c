#include "true_vertical_profile.h"
#include <stdio.h>
#include "math.h"
#include "material_coords_raincell.h"
#include "common.h"


// local variables
double height_base = 1000;
double height_max_echo_top = 8000;
double height_brightband = 3500;
double width_brightband = 1000;
//double height_brightband_l = height_brightband-width_brightband;
double height_brightband_l;
//double height_brightband_u = height_brightband+width_brightband;
double height_brightband_u;
double reflectivity_base = 30;
double reflectivity_max_echo_top = 20;
double reflectivity_brightband = 60;
double reflectivity_absdiff_brightband = 15;
//double reflectivity_brightband_l = height_brightband-reflectivity_absdiff_brightband*2/3;
double reflectivity_brightband_l;
//double reflectivity_brightband_u = height_brightband-reflectivity_absdiff_brightband/3;
double reflectivity_brightband_u;
//double gradient_base_to_bottom_T0 = (height_brightband_l - height_base) / (reflectivity_brightband_l - reflectivity_base);
double gradient_base_to_bottom_T0;
//double gradient_bottom_T0_to_T0 = (height_brightband - height_brightband_l) / (reflectivity_brightband - reflectivity_brightband_l);
double gradient_bottom_T0_to_T0;
//double gradient_T0_to_top_T0 = (height_brightband_u - height_brightband) / (reflectivity_brightband_u - reflectivity_brightband);
double gradient_T0_to_top_T0;
//double gradient_top_T0_to_max_echo_top = (height_max_echo_top - height_brightband_u) / (reflectivity_max_echo_top - reflectivity_brightband_u);
double gradient_top_T0_to_max_echo_top;



// function retrieving the reflectivity at a given height for the vertical profile of reflectivity. 

double reflectivity(double height){
	if (height<height_base || height>height_max_echo_top){
		return 0.0;
	}
	else if (height_base <= height && height < height_brightband_l) {
		return (1/gradient_base_to_bottom_T0)*height + reflectivity_base;
	}
	else if (height_brightband_l <= height && height < height_brightband) {
                return (1/gradient_bottom_T0_to_T0)*height + reflectivity_brightband_l;
        }
	else if (height_brightband <= height && height < height_brightband_u) {
                return (1/gradient_T0_to_top_T0)*height + reflectivity_brightband;
        }
	else if (height_brightband_u <= height && height < height_max_echo_top) {
                return (1/gradient_top_T0_to_max_echo_top)*height + reflectivity_brightband_u;
        }

	return -1.0;
}
