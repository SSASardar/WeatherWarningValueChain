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
	double min_range_gate;
	double max_range_gate;
	double min_angle;
	double max_angle;
	double num_ranges;
	double num_angles;
	double range_resolution;
	double angular_resolution;
 	double *data_array;	
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
        printf("Radar is not found (pointing to NULL).\n");
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



Point* get_position_radar(const Radar* radar){
	Point* point = malloc(sizeof(Point));
	if (radar) {
		point->x = radar->x;
		point->y = radar->y;
	}	
	return point;
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

double get_max_range_radar(const Radar* radar){
	return radar->maximum_range;
}	


const char* get_frequency(const Radar* r) {
    return r->frequency;
}

const char* get_scanning_mode(const Radar* r) {
    return r->scanning_mode;
}

// Figuring out the number of range gates 
Polar_box* create_polar_box(double time, const Spatial_raincell* s_raincell, const Radar* radar, const Raincell* raincell){
    	
	if (strcmp(get_scanning_mode(radar),"PPI")!=0) {
		printf("create_polar_box:\nAt simulation time %.2lf you are trying to create a polar box for a %s\nPolar box NOT made: assigning NULL\n\n", time, get_scanning_mode(radar));
		return NULL;
	}
	
	Point* centre = get_position_raincell(time, s_raincell);

	Point* radar_point = get_position_radar(radar);


	Polar_box* polar_box = malloc(sizeof(Polar_box));

	double diff_x = centre->x-radar_point->x;
	double diff_y = centre->y-radar_point->y;

	double dist = sqrt((diff_x)*(diff_x) + (diff_y)*(diff_y) );
	double radius_stratiform = raincell_get_radius_stratiform(raincell);
	polar_box->range_resolution= get_range_res_radar(radar);
	double range_resolution = polar_box->range_resolution;

	double angle = atan2(diff_y,diff_x);
	if (angle<0){
		angle += 2*M_PI;
	}

	// Convert angle from radians to degrees
	angle = angle * RAD2DEG;

	double del_angle = atan2(radius_stratiform,dist);
	
	
	// Convert del_angle from radians to degrees
	del_angle = del_angle * RAD2DEG;
	
	polar_box->angular_resolution = get_angular_res_radar(radar); // is already in degreees.
	double angular_resolution = polar_box->angular_resolution;





	polar_box->radar_id = get_radar_id(radar);
	polar_box->min_range_gate = floor((dist-radius_stratiform)/range_resolution);
	//polar_box->min_range = floor((dist-radius_stratiform));
	polar_box->max_range_gate = ceil((dist+radius_stratiform)/range_resolution);
	//polar_box->max_range = ceil((dist+radius_stratiform));

	polar_box->min_angle = floor((angle-del_angle)/angular_resolution);
	polar_box->max_angle = ceil((angle+del_angle)/angular_resolution);
	
	polar_box->num_ranges = polar_box->max_range_gate - polar_box->min_range_gate;
	polar_box->num_angles = polar_box->max_angle - polar_box->min_angle;
	free(centre);
	free(radar_point);

	polar_box->data_array = NULL;

	return polar_box;
}



int get_radar_id_for_polar_box(const struct Polar_box* box) {
    return box->radar_id;
}

double get_min_range_gate(const struct Polar_box* box) {
    return box->min_range_gate;
}

double get_max_range_gate(const struct Polar_box* box) {
    return box->max_range_gate;
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

// Function to find Radar by ID              
const Radar* find_radar_by_id(const Polar_box* box, const Radar** radars, int num_radars) {
    for (int i = 0; i < num_radars; ++i) {   
        if (radars[i]->id == get_radar_id_for_polar_box(box)) {
            return radars[i];
        }
    }
    printf("find_radar_by_id()\nRadar with id %d was not found. found_radar is allocated to NULL\n\n", get_radar_id_for_polar_box(box));
    return NULL; // Not found

}

void print_polar_grid(const Polar_box* box, const Radar** radars, int num_radars) {
if(box==NULL){printf("polar box not found (points to NULL) in print_polar_grid function.\n\n");}
  const Radar* found_radar = find_radar_by_id(box, radars, num_radars);

    // Print polar box info
    printf("Radar ID      : %d\n", get_radar_id_for_polar_box(box));
    printf("Num of Angles : %.0f\n", get_num_angles(box));
    printf("Num of Ranges : %.0f\n", get_num_ranges(box));
    printf("Between angles: [%.0f, %.0f]\n", get_max_angle(box), get_min_angle(box));
    printf("Between ranges: [%.0f, %.0f]\n", get_min_range_gate(box)*get_range_res_radar(found_radar), get_max_range_gate(box)*get_range_res_radar(found_radar));
    printf("\n");

}

Bounding_box* create_bounding_box_for_polar_box(const Polar_box* p_box, const Radar** radars, int num_radars){
if(p_box==NULL){printf("create_bounding+box_for_polar_plot\n You are trying to create a bounding box for a polar box which is not defined (points to NULL)\n The bounding box will be assigned NULL\n\n");return NULL;}
	
	const Radar* found_radar = find_radar_by_id(p_box, radars, num_radars);

double rmin = get_min_range_gate(p_box) * get_range_res_radar(found_radar);
double rmax = get_max_range_gate(p_box) * get_range_res_radar(found_radar);
double anglemin = get_min_angle(p_box) * DEG2RAD;
double anglemax = get_max_angle(p_box) * DEG2RAD;
double anglemid = (anglemin + anglemax) / 2;

double xs[5] = {
    rmax * cos(anglemin),
    rmin * cos(anglemin),
    rmax * cos(anglemax),
    rmin * cos(anglemax),
    rmax * cos(anglemid)
};

double ys[5] = {
    rmax * sin(anglemin),
    rmin * sin(anglemin),
    rmax * sin(anglemax),
    rmin * sin(anglemax),
    rmax * sin(anglemid)
};


double xmin = xs[0], xmax = xs[0];
double ymin = ys[0], ymax = ys[0];

for (int i = 1; i < 5; ++i) {
    if (xs[i] < xmin) xmin = xs[i];
    if (xs[i] > xmax) xmax = xs[i];
    if (ys[i] < ymin) ymin = ys[i];
    if (ys[i] > ymax) ymax = ys[i];
}

Point* pos_radar = get_position_radar(found_radar);


// Allocate and fill the bounding box
Bounding_box* bbox = malloc(sizeof(Bounding_box));
bbox->topLeft.x = xmin+pos_radar->x;
bbox->topLeft.y = ymax+pos_radar->y;

bbox->topRight.x = xmax+pos_radar->x;
bbox->topRight.y = ymax+pos_radar->y;

bbox->bottomLeft.x = xmin+pos_radar->x;
bbox->bottomLeft.y = ymin+pos_radar->y;

bbox->bottomRight.x = xmax+pos_radar->x;
bbox->bottomRight.y = ymin+pos_radar->y;

return bbox;


}

