// This is the header file for the vertical profiles.c file
//
//
// The setup of all vertical information is done here. In the processing file, in the radars.c file the measuremnt noise can be included. 
//







#ifndef VERTICAL_PROFILES_H
#define VERTICAL_PROFILES_H

typedef struct VPR_point {
	double reflectivity;
	double height;
} VPR_point;


typedef struct VPR {
	VPR_point ET;
	VPR_point BB_u;
	VPR_point BB_m;
	VPR_point BB_l;
	VPR_point CB;
} VPR;

typedef struct VPR_params {
	//time parameters
	double t_growth_start;
	double t_mature_start;
	double t_mature_end;
	double t_decay_mid;
	double t_decay_end;

	double div_f_growth;
	double div_f_mature;
	double div_f_decay;
	double div_f_decay1;
	double div_f_decay2;

	// Echo Top parameters
	double Z_et_0;
	double h_et_0;
	double del_h_et;

	// bright band parameters
	double Z_bb_0;
	double del_Z_bb_growth;
	double del_Z_bb_mature;
	double del_Z_bb_decay;
	double h_bb_0;
	double del_h_bb_growth;
	double del_h_bb_mature;
	double width_Z_0;
	double del_width_Z_growth;
	double del_width_Z_mature;
	double width_h_0;
        double del_width_h_growth;
	double del_width_h_mature;


	double ratio_U_to_L;
	double del_ratio_UL_growth;
	double del_ratio_UL_mature;
	double del_ratio_UL_decay;

	// cell base parameters
	double Z_cb_0;
	double del_Z_cb_growth;
	double del_Z_cb_mature;
	double del_Z_cb_decay;
	double h_cb_0;
	double del_h_cb_growth;
	double del_h_cb_mature;
		
} VPR_params;


// Assume these are ordered lowest to highest heights:
VPR_point sorted_points[5];


// Initializes all fields of VPR_params to zero
void init_VPR_params(VPR_params *params);

// Fills all fields of VPR_params with provided values
void fill_VPR_params(
    VPR_params *params,
    double t_growth_start, double t_mature_start, double t_mature_end,
    double t_decay_mid, double t_decay_end,
    double Z_et_0, double h_et_0, double del_h_et,
    double Z_bb_0, double del_Z_bb_growth, double del_Z_bb_mature, double del_Z_bb_decay,
    double h_bb_0, double del_h_bb_growth, double del_h_bb_mature,
    double width_Z_0, double del_width_Z_growth, double del_width_Z_mature,
    double width_h_0, double del_width_h_growth, double del_width_h_mature,
    double ratio_U_to_L, double del_ratio_UL_growth, double del_ratio_UL_mature, double del_ratio_UL_decay,
    double Z_cb_0, double del_Z_cb_growth, double del_Z_cb_mature, double del_Z_cb_decay,
    double h_cb_0, double del_h_cb_growth, double del_h_cb_mature
);

VPR *create_VPR(void);


VPR *create_and_fill_VPR(const VPR_params *params);


void update_VPR(const VPR *vpr, const VPR_params *params, double time, VPR *vpr_conv);

void print_VPR_points(const VPR* vpr);

double interpolate_reflectivity(VPR_point p1, VPR_point p2, double height);

double get_reflectivity_at_height(VPR *vpr, double height);

void cumaddVPR(const VPR *src, VPR *dest);

void divideVPR(VPR *vpr, int divisor);	

void cumaddVPR_scale(const VPR *src, VPR *dest, double scale);

void multiplyVPR(VPR *vpr, double scalar);

void compute_average_VPR(VPR *vpr_avg, VPR_params *params,double t_start,double t_end,double dt,VPR *scratch);
void zeroVPR(VPR *vpr);
void compute_climatology_VPR(VPR *vpr_clima, VPR_params *params, double dt, double strat_tail, VPR *scratch);


#endif // VERTICAL_PROFILES_H
