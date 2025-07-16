#include "true_vertical_profile.c"
#include <stdio.h>
#include "math.h"


extern height;

\\ local variables
static double height_base = 1000;
static double height_max_echo_top = 8000;


static double height_brightband = 3500;
static double width_brightband = 1000;

static double height_brightband_l = height_brightband-width_brightband;
static double height_brightband_u = height_brightband+width_brightband;


static double reflectivity_base = 30;
static double reflectivity_max_echo_top = 20;


static double reflectivity_brightband = 60;
static double reflectivity_absdiff_brightband = 15;

static double reflectivity_brightband_l = height_brightband-reflectivity_absdiff_brightband*2/3;
static double reflectivity_brightband_u = height_brightband-reflectivity_absdiff_brightband/3;


static double gradient_base_to_bottom_T0 = (height_brightband_l - height_base) / (reflectivity_brightband_l - reflectivity_base);
static double gradient_bottom_T0_to_T0 = (height_brightband - height_brightband_l) / (reflectivity_brightband - reflectivity_brightband_l);
static double gradient_T0_to_top_T0 = (height_brightband_u - height_brightband) / (reflectivity_brightband_u - reflectivity_brightband);
static double gradient_top_T0_to_max_echo_top = (height_max_echo_top - height_brightband_u) / (reflectivity_max_echo_top - reflectivity_brightband_u);



\\ function retrieving the reflectivity at a given height for the vertical profile of reflectivity. 

double reflectivity(height){
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



}
