#include <stdio.h>
#include "vertical_profiles.h"
#include <stdlib.h>

// Function to initialize all fields to zero
void init_VPR_params(VPR_params *params) {
    if (!params) return; // safety check
    *params = (VPR_params){0}; // C99 compound literal zero-initialization
}

// Function to fill all fields with provided values
// Each value must be passed explicitly to avoid uninitialized members
void fill_VPR_params(
    VPR_params *params,
    double t_growth_start, double t_mature_start, double t_mature_end,
    double t_decay_mid, double t_decay_end,
    double Z_et_0, double h_et_0, double del_h_et,
    double Z_bb_0, double del_Z_bb_growth, double del_Z_bb_mature, double del_Z_bb_decay,
    double h_bb_0, double del_h_bb_growth, double del_h_bb_mature,
    double width_Z_0, double del_width_Z_growth, double del_width_Z_mature,
    double width_h_0, double del_width_h_growth, double del_width_h_mature,
    double ratio_U_to_L,
    double Z_cb_0, double del_Z_cb_growth, double del_Z_cb_mature, double del_Z_cb_decay,
    double h_cb_0, double del_h_cb_growth, double del_h_cb_mature
) {
    if (!params) return;

    params->t_growth_start = t_growth_start;
    params->t_mature_start = t_mature_start;
    params->t_mature_end   = t_mature_end;
    params->t_decay_mid    = t_decay_mid;
    params->t_decay_end    = t_decay_end;

    params->div_f_growth = 1.0/(params->t_mature_start - params->t_growth_start);
    params->div_f_mature = 1.0/(params->t_mature_end - params->t_mature_start);
    params->div_f_decay = 1.0/(params->t_decay_end - params->t_mature_end);
    params->div_f_decay1 = 1.0/(params->t_decay_mid - params->t_mature_end);
    params->div_f_decay2 = 1.0/(params->t_decay_end - params->t_decay_mid);

    params->Z_et_0   = Z_et_0;
    params->h_et_0   = h_et_0;
    params->del_h_et = del_h_et;

    params->Z_bb_0          = Z_bb_0;
    params->del_Z_bb_growth = del_Z_bb_growth;
    params->del_Z_bb_mature = del_Z_bb_mature;
    params->del_Z_bb_decay  = del_Z_bb_decay;
    params->h_bb_0          = h_bb_0;
    params->del_h_bb_growth = del_h_bb_growth;
    params->del_h_bb_mature = del_h_bb_mature;

    params->width_Z_0          = width_Z_0;
    params->del_width_Z_growth = del_width_Z_growth;
    params->del_width_Z_mature = del_width_Z_mature;
    params->width_h_0      = width_h_0;
    params->del_width_h_growth = del_width_h_growth;
    params->del_width_h_mature = del_width_h_mature;

    params->ratio_U_to_L = ratio_U_to_L;

    params->Z_cb_0          = Z_cb_0;
    params->del_Z_cb_growth = del_Z_cb_growth;
    params->del_Z_cb_mature = del_Z_cb_mature;
    params->del_Z_cb_decay  = del_Z_cb_decay;
    params->h_cb_0          = h_cb_0;
    params->del_h_cb_growth = del_h_cb_growth;
    params->del_h_cb_mature = del_h_cb_mature;
}


VPR *create_and_fill_VPR(const VPR_params *params) {
    if (!params) {printf("VPR_params is NULL\n");return NULL;}

    VPR *vpr = malloc(sizeof(VPR));
    if (!vpr) return NULL;

    // Fill Echo Top (ET)
    vpr->ET.reflectivity = params->Z_et_0;
    vpr->ET.height       = params->h_et_0;

    // Bright Band Middle
    vpr->BB_m.reflectivity = params->Z_bb_0;
    vpr->BB_m.height       = params->h_bb_0;

    // Bright band upper
    vpr->BB_u.reflectivity = vpr->BB_m.reflectivity - params->width_Z_0 *(1- params->ratio_U_to_L);
    vpr->BB_u.height = vpr->BB_m.height + params->width_h_0 * params->ratio_U_to_L;

    // Bright_band lower
    vpr->BB_l.reflectivity = vpr->BB_m.reflectivity - params->width_Z_0 * (params->ratio_U_to_L);
    vpr->BB_l.height = vpr->BB_m.height - params->width_h_0 * (1-params->ratio_U_to_L);



    // Cell Base
    vpr->CB.reflectivity = params->Z_cb_0;
    vpr->CB.height       = params->h_cb_0;

    return vpr;
}
void update_VPR(const VPR *vpr, const VPR_params *params, double time, VPR *vpr_conv) {
    if (!vpr || !params || !vpr_conv) return;

    // Precompute interpolation factors (clamped between 0 and 1)
    double f_growth   = 0.0;
    double f_mature   = 0.0;
    double f_decay    = 0.0; // single decay factor if using combined range
    double f_decay1   = 0.0; // early decay factor
    double f_decay2   = 0.0; // late decay factor


    double width_Z = 0.00;
    double width_h = 0.00;

   // if (params->t_mature_start > params->t_growth_start)
        f_growth = (time - params->t_growth_start) * params->div_f_growth;

//	if (time == 90.0*60) printf("time = %.2f, \n t_growth_start = %.2f, \n divisor = %.2f\n\n", time, params->t_growth_start, params->div_f_growth);
   // if (params->t_mature_end > params->t_mature_start)
        f_mature = (time - params->t_mature_start) * params->div_f_mature;

   // if (params->t_decay_end > params->t_mature_end)
        f_decay = (time - params->t_mature_end) * params->div_f_decay;

    // if (params->t_decay_mid > params->t_mature_end)
        f_decay1 = (time - params->t_mature_end) * params->div_f_decay1;

    // if (params->t_decay_end > params->t_decay_mid)
        f_decay2 = (time - params->t_decay_mid) * params->div_f_decay2;

    // Clamp factors to [0, 1] for safety
    if (f_growth < 0) f_growth = 0; if (f_growth > 1) f_growth = 1;
    if (f_mature < 0) f_mature = 0; if (f_mature > 1) f_mature = 1;
    if (f_decay  < 0) f_decay  = 0; if (f_decay  > 1) f_decay  = 1;
    if (f_decay1 < 0) f_decay1 = 0; if (f_decay1 > 1) f_decay1 = 1;
    if (f_decay2 < 0) f_decay2 = 0; if (f_decay2 > 1) f_decay2 = 1;
	
    // Echo top 
	vpr_conv->ET.reflectivity = vpr->ET.reflectivity; // nonchanging reflectivity.
	vpr_conv->ET.height = vpr->ET.height + (f_growth * params->del_h_et)*(1.0-f_decay); //height rises with growth, and falls when decaying

    // Bright band middle
	vpr_conv->BB_m.reflectivity = vpr->BB_m.reflectivity + (f_growth * params->del_Z_bb_growth + f_mature * params->del_Z_bb_mature + f_decay1 * params->del_Z_bb_decay)*(1.0-f_decay2);
	vpr_conv->BB_m.height = vpr->BB_m.height + (f_growth * params->del_h_bb_growth + f_mature * params->del_h_bb_mature)*(1.0-f_decay);
	
	width_Z = params->width_Z_0 + (f_growth * params->del_width_Z_growth + f_mature * params->del_width_Z_mature)*(1.0-f_decay);
	width_h = params->width_h_0 + (f_growth * params->del_width_h_growth + f_mature * params->del_width_h_mature)*(1.0-f_decay);

    // Bright band upper
    	vpr_conv->BB_u.reflectivity = vpr_conv->BB_m.reflectivity - (width_Z) * (1-params->ratio_U_to_L);
	vpr_conv->BB_u.height = vpr_conv->BB_m.height + (width_h) * (1-params->ratio_U_to_L);

    // Bright band lower:
    	vpr_conv->BB_l.reflectivity = vpr_conv->BB_m.reflectivity - (width_Z) * (params->ratio_U_to_L);
	vpr_conv->BB_l.height = vpr_conv->BB_m.height - (width_h) * (params->ratio_U_to_L);
    	
    // Cell base:
    
	vpr_conv->CB.reflectivity = vpr->CB.reflectivity + (f_growth * params->del_Z_cb_growth + f_mature * params->del_Z_cb_mature + f_decay1 * params->del_Z_cb_decay)*(1.0-f_decay2);
	vpr_conv->CB.height = vpr->CB.height + (f_growth * params->del_h_cb_growth + f_mature * params->del_h_cb_mature)*(1.0-f_decay);


//printf("scaling factors are (f_growth, f_mature, f_decay, f_decay1, f_decay2)\n			(%.2f , %.2f , %.2f , %.2f , %.2f)\n", f_growth, f_mature, f_decay, f_decay1, f_decay2);
}




void print_VPR_points(const VPR* vpr) {
    printf("(reflectivty, height)  \n");
    printf("ET:     (%.2f , %.2f)\n", vpr->ET.reflectivity, vpr->ET.height);
    printf("BB_u:   (%.2f , %.2f)\n", vpr->BB_u.reflectivity, vpr->BB_u.height);
    printf("BB_m:   (%.2f , %.2f)\n", vpr->BB_m.reflectivity, vpr->BB_m.height);
    printf("BB_l:   (%.2f , %.2f)\n", vpr->BB_l.reflectivity, vpr->BB_l.height);
    printf("CB:     (%.2f , %.2f)\n", vpr->CB.reflectivity, vpr->CB.height);
}


double interpolate_reflectivity(VPR_point p1, VPR_point p2, double height) {
    // Linear interpolation formula
    return p1.reflectivity + (height - p1.height) * 
           (p2.reflectivity - p1.reflectivity) / (p2.height - p1.height);
}


double get_reflectivity_at_height(VPR *vpr, double height) {
    // Order points from lowest to highest height
    VPR_point sorted_points[5] = {vpr->CB, vpr->BB_l, vpr->BB_m, vpr->BB_u, vpr->ET};

    // If height is below lowest point, return reflectivity at lowest
    if (height <= sorted_points[0].height) {
       // return sorted_points[0].reflectivity;
        return 0.0;
    }

    // If height is above highest point, return reflectivity at highest
    if (height >= sorted_points[4].height) {
       // return sorted_points[4].reflectivity;
    	return 0.0;
    }

    // Find interval where height fits and interpolate
    for (int i = 0; i < 4; i++) {
        double h1 = sorted_points[i].height;
        double h2 = sorted_points[i + 1].height;

        if (height >= h1 && height <= h2) {
            return interpolate_reflectivity(sorted_points[i], sorted_points[i + 1], height);
        }
    }

    // Should never reach here if input height is in range
    return -1.0;
}













