// This is the radar.c file
// 	It defines the structure of a radar
// 	It can execute scanning instructions



// include statements:
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "radars.h"
#include "spatial_coords_raincell.h"
#include "material_coords_raincell.h"
#include "common.h"


// Defining a radar

struct Radar {
	int id;
	char frequency[2];
	char scanning_mode[4];
	double x;
	double y;
	double z;
	double maximum_range;
	double range_resolution;
	double angular_resolution;
};

struct Polar_box {
	int radar_id;
	double min_range;
	double max_range;
	double min_angle;
	double max_angle;
	double num_ranges;
	double num_angles; 
};

//Creating a radar
Radar* create_radar(int id, const char* frequency, const char* scanning_mode,
                    double x, double y, double z,  double max_range,
                    double range_res, double angular_res) {
    Radar* r = (Radar*)malloc(sizeof(Radar));
    if (r == NULL) {
        printf("Memory allocation failed.\n");
        return NULL;
    }

    r->id = id;
    strncpy(r->frequency, frequency, 1);
    r->frequency[1] = '\0';

    strncpy(r->scanning_mode, scanning_mode, 3);
    r->scanning_mode[3] = '\0';

    r->x = x;
    r->y = y;
    r->z = z;
    r->maximum_range = max_range;
    r->range_resolution = range_res;
    r->angular_resolution = angular_res;

    return r;
}


// To check the radar setup
void print_radar_specs(const Radar* radar) {
    if (radar == NULL) {
        printf("Radar is NULL.\n");
        return;
    }

    printf("Radar ID: %d\n", radar->id);
    printf("Frequency: %s\n", radar->frequency);
    printf("Scanning Mode: %s\n", radar->scanning_mode);
    printf("Position (x,y,z): (%.2f, %.2f, %.2lf)\n", radar->x, radar->y, radar->z);  // Removed radar->z since it's not defined
    printf("Max Range: %.2f\n", radar->maximum_range);
    printf("Range Res: %.2f\n", radar->range_resolution);
    printf("Angular Res: %.2f\n", radar->angular_resolution);
}



void get_position_radar(const Radar* radar, double *x_out, double *y_out){
	if (radar && x_out && y_out) {
		*x_out = radar->x;
		*y_out = radar->y;
	}	
}

double get_range_res_radar(const Radar* radar){
	return radar->range_resolution;
}

double get_angular_res_radar(const Radar* radar){
	return radar->angular_resolution;
}

int get_radar_id(const Radar* radar){
	return radar->id;
}

// Figuring out the number of range gates 
Polar_box* create_polar_box(double time, const Spatial_raincell* s_raincell, const Radar* radar, const Raincell* raincell){
	double centre_x,centre_y;
	get_position_raincell(time, s_raincell, &centre_x, &centre_y);

	double radar_x, radar_y;
	get_position_radar(radar, &radar_x, &radar_y);
	
	double diff_x = centre_x-radar_x;
	double diff_y = centre_y-radar_y;

	double dist = sqrt((diff_x)*(diff_x) + (diff_y)*(diff_y) );
	double radius_stratiform = raincell_get_radius_stratiform(raincell);
	double range_resolution = get_range_res_radar(radar);
	

	double angle = atan2(diff_y,diff_x);
	if (angle<0){
		angle += 2*M_PI;
	}
	
	double del_angle = atan2(radius_stratiform,dist);
	double angular_resolution = get_angular_res_radar(radar);
	
	Polar_box* polar_box = malloc(sizeof(Polar_box));

	polar_box->radar_id = get_radar_id(radar);
	polar_box->min_range = floor((dist-radius_stratiform)/range_resolution);
	polar_box->max_range = ceil((dist+radius_stratiform)/range_resolution);

	polar_box->min_angle = floor((angle-del_angle)/angular_resolution);
	polar_box->max_angle = ceil((angle+del_angle)/angular_resolution);
	
	polar_box->num_ranges = polar_box->max_range - polar_box->min_range;
	polar_box->num_angles = polar_box->max_angle - polar_box->min_angle;

	return polar_box;
}



int get_radar_id_for_polar_box(const struct Polar_box* box) {
    return box->radar_id;
}

double get_min_range(const struct Polar_box* box) {
    return box->min_range;
}

double get_max_range(const struct Polar_box* box) {
    return box->max_range;
}

double get_min_angle(const struct Polar_box* box) {
    return box->min_angle;
}

double get_max_angle(const struct Polar_box* box) {
    return box->max_angle;
}

double get_num_ranges(const struct Polar_box* box) {
    return box->num_ranges;
}

double get_num_angles(const struct Polar_box* box) {
    return box->num_angles;
}




void print_polar_grid(const Polar_box* box) {
    int i;
    int width = 20;  // Width of the angle line
    int num_lines = 5;

    // Print radar info
    printf("Radar ID      : %d\n", get_radar_id_for_polar_box(box));
    printf("Num of Angles : %.0f\n", get_num_angles(box));
    printf("Num of Ranges : %.0f\n", get_num_ranges(box));
    printf("\n");

    // Print angle header
    printf("Max_angle (%.2f°)%*sMin_angle (%.2f°)\n", 
           get_max_angle(box), width, "", get_min_angle(box));

    printf("%.*s| max_range (%.2f)\n", width, "________________________________________", get_max_range(box));
    // Print grid lines
    for (i = 0; i < num_lines; ++i) {
        printf("%.*s|\n", width, "________________________________________");
    }

    // Print bottom with range labels
    printf("%.*s| min_range (%.2f)\n", width, "________________________________________", get_min_range(box));
}
