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
#include <ctype.h>
#include <assert.h>
// Defining a radar
/*
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
 	double *grid;	
	double other_angle;
};

struct RadarScan {
	int scan_index;
	Radar* radar;
	Polar_box* box;
};

*/

// Global registries:
// // Global radar registry to deduplicate
Radar* radar_list[MAX_RADARS];
int radar_count = 0;

// Scan collection
RadarScan radar_scans[MAX_SCANS];
int scan_count = 0;


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


Radar* get_or_create_radar(int id, const char* freq, const char* mode,double x, double y, double z, double max_range,double range_res, double angular_res) {
    for (int i = 0; i < radar_count; ++i) {
        if (radar_list[i]->id == id)
            return radar_list[i];
    }

    Radar* r = create_radar(id, freq, mode, x, y, z, max_range, range_res, angular_res);
    if (r && radar_count < MAX_RADARS)
        radar_list[radar_count++] = r;
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
double get_height_of_radar(const Radar* radar){
return radar->z;
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
/*
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

	polar_box->grid = NULL;

	return polar_box;
}

*/

/*
Polar_box* create_polar_box(
    int radar_id,
    double min_range_gate,
    double max_range_gate,
    double min_angle,
    double max_angle,
    double num_ranges,
    double num_angles,
    double range_res,
    double angular_res,
    int grid_size,
    double other_angle,
    double *grid_data,
    int height_size,
    double *height_data
) {
    // function body
    Polar_box* box = (Polar_box*)malloc(sizeof(Polar_box));
    if (!box) {
        printf("Memory allocation failed for box.\n");
        return NULL;
    }

    box->radar_id = radar_id;
    box->min_range_gate = min_range_gate;
    box->max_range_gate = max_range_gate;
    box->min_angle = min_angle;
    box->max_angle = max_angle;
    box->num_ranges = num_ranges;
    box->num_angles = num_angles;
    box->range_resolution = range_res;
    box->angular_resolution = angular_res;
    box->other_angle = other_angle;

    if (grid_size > 0 && grid_data != NULL) {
        box->grid = (double*)malloc(sizeof(double) * grid_size);
        box->attenuation_grid = (double*)malloc(sizeof(double) * 3 * grid_size);
	if (!box->grid) {
            printf("Grid allocation failed.\n");
            free(box);
            return NULL;
        }
        memcpy(box->grid, grid_data, sizeof(double) * grid_size);
    } else {
        box->grid = NULL;
    }

if (height_size > 0 && height_data != NULL) {
    box->height_grid = (double*)malloc(sizeof(double) * height_size);
    if (!box->height_grid) {
        printf("Grid allocation failed. for height\n");
        free(box);
        return NULL;
    }
    memcpy(box->height_grid, height_data, sizeof(double) * height_size);
} else {
    box->grid = NULL;
}
double temp_var;
int idg, idag, idagc;
for(int p = 0; p<num_ranges;p++){
	for (int q = 0; q<num_angles;q++){
		idg = p*num_angles + q;
		idag = idg*3;
		temp_var = box->grid[idg];
		if (temp_var == 0.0) box->attenuation_grid[idag + 0] ++;
		if (temp_var == 1.0) box->attenuation_grid[idag + 1] ++;
		if (temp_var == 2.0) box->attenuation_grid[idag + 2] ++;
		
		if (p>0){
			idagc = 3*(p-1)*num_angles + 3*q;
			box->attenuation_grid[idag + 0] = box->attenuation_grid[idag + 0] + box->attenuation_grid[idagc+0];
			box->attenuation_grid[idag + 1] = box->attenuation_grid[idag + 1] + box->attenuation_grid[idagc+1];
			box->attenuation_grid[idag + 2] = box->attenuation_grid[idag + 2] + box->attenuation_grid[idagc+2];
		}

	}
}

    return box;
}
*/

Polar_box* create_polar_box(
    int radar_id,
    double min_range_gate,
    double max_range_gate,
    double min_angle,
    double max_angle,
    double num_ranges,
    double num_angles,
    double range_res,
    double angular_res,
    int grid_size,
    double other_angle,
    double *grid_data,
    int height_size,
    double *height_data
) {
    Polar_box* box = (Polar_box*)malloc(sizeof(Polar_box));
    if (!box) {
        printf("Memory allocation failed for Polar_box.\n");
        return NULL;
    }

    // Initialize all pointers to NULL
    box->grid = NULL;
    box->attenuation_grid = NULL;
    box->height_grid = NULL;

    // Set scalar values
    box->radar_id = radar_id;
    box->min_range_gate = min_range_gate;
    box->max_range_gate = max_range_gate;
    box->min_angle = min_angle;
    box->max_angle = max_angle;
    box->num_ranges = num_ranges;
    box->num_angles = num_angles;
    box->range_resolution = range_res;
    box->angular_resolution = angular_res;
    box->other_angle = other_angle;

    // Allocate grid and attenuation_grid if needed
    if (grid_size > 0 && grid_data != NULL) {
        box->grid = (double*)malloc(sizeof(double) * grid_size);
        box->attenuation_grid = (double*)malloc(sizeof(double) * 3 * grid_size);

        if (!box->grid || !box->attenuation_grid) {
            printf("Grid allocation failed.\n");
            free(box->grid);
            free(box->attenuation_grid);
            free(box);
            return NULL;
        }

        memcpy(box->grid, grid_data, sizeof(double) * grid_size);
        memset(box->attenuation_grid, 0, sizeof(double) * 3 * grid_size); // initialize to 0
    }

    // Allocate height_grid if needed
    if (height_size > 0 && height_data != NULL) {
        box->height_grid = (double*)malloc(sizeof(double) * height_size);
        if (!box->height_grid) {
            printf("Height grid allocation failed.\n");
            free(box->grid);
            free(box->attenuation_grid);
            free(box);
            return NULL;
        }
        memcpy(box->height_grid, height_data, sizeof(double) * height_size);
    }

    // Compute attenuation grid cumulative sum
    if (box->grid && box->attenuation_grid) {
        for (int p = 0; p < (int)num_ranges; p++) {
            for (int q = 0; q < (int)num_angles; q++) {
                int idg = p * (int)num_angles + q;
                int idag = idg * 3;
                double val = box->grid[idg];

                // Increment corresponding attenuation
                if (val == 0.0) box->attenuation_grid[idag + 0] += 1.0;
                if (val == 1.0) box->attenuation_grid[idag + 1] += 1.0;
                if (val == 2.0) box->attenuation_grid[idag + 2] += 1.0;

                // Add cumulative from previous row if not first row
                if (p > 0) {
                    int idagc = 3 * ((p - 1) * (int)num_angles + q);
                    box->attenuation_grid[idag + 0] += box->attenuation_grid[idagc + 0];
                    box->attenuation_grid[idag + 1] += box->attenuation_grid[idagc + 1];
                    box->attenuation_grid[idag + 2] += box->attenuation_grid[idagc + 2];
                }
            }
        }
    }

    return box;
}



Polar_box* init_polar_box() {
    Polar_box* polar_box = malloc(sizeof(Polar_box));
    if (!polar_box) {
        fprintf(stderr, "Failed to allocate Polar_box\n");
        return NULL;
    }
    // Initialize pointers to NULL or zero out members as needed
    polar_box->grid = NULL;
    polar_box->height_grid = NULL;
    polar_box->attenuation_grid = NULL;
    // Initialize other members to sensible defaults, e.g. 0
    polar_box->range_resolution = 0.0;
    polar_box->angular_resolution = 0.0;
    polar_box->min_range_gate = 0;
    polar_box->max_range_gate = 0;
    polar_box->min_angle = 0;
    polar_box->max_angle = 0;
    polar_box->num_ranges = 0;
    polar_box->num_angles = 0;
    polar_box->radar_id = -1; // or any invalid default
polar_box->other_angle = 0*DEG2RAD;
    return polar_box;
}

void update_other_angle(Polar_box* p_box, double new_angle){

	if(p_box == NULL){printf("in radars.c the updating angle failed because of a NULL polar box\n"); return;}

	p_box->other_angle = new_angle;
}
/*
int fill_polar_box(Polar_box* polar_box, double time, const Spatial_raincell* s_raincell, const Radar* radar, const Raincell* raincell) {
    if (!polar_box) return -1;

    if (strcmp(get_scanning_mode(radar), "PPI") != 0) {
        printf("fill_polar_box:\nAt simulation time %.2lf you are trying to fill a polar box for a %s\nPolar box NOT updated.\n\n", time, get_scanning_mode(radar));
        return -1;
    }
	
    // earth 4/3 mean earth model. in calculating slant range.
    double kea_and_radar = KEA + radar->z;

    Point* centre = get_position_raincell(time, s_raincell);
    Point* radar_point = get_position_radar(radar);

    double diff_x = centre->x - radar_point->x;
    double diff_y = centre->y - radar_point->y;
    double dist_s = sqrt(diff_x * diff_x + diff_y * diff_y);
    double dist = sin(dist_s/kea_and_radar)*(kea_and_radar)/cos(polar_box->other_angle);
    double radius_stratiform = raincell_get_radius_stratiform(raincell);

    polar_box->range_resolution = get_range_res_radar(radar);
    double range_resolution = polar_box->range_resolution;

    double angle = atan2(diff_y, diff_x);
    if (angle < 0) angle += 2 * M_PI;
    angle = angle * RAD2DEG;

    double del_angle = atan2(radius_stratiform, dist);
    del_angle = del_angle * RAD2DEG;

    polar_box->angular_resolution = get_angular_res_radar(radar);
    double angular_resolution = polar_box->angular_resolution;

    polar_box->radar_id = get_radar_id(radar);
    polar_box->min_range_gate = floor((sin((dist_s - radius_stratiform)/kea_and_radar)*kea_and_radar/cos(polar_box->other_angle)) / range_resolution);
    polar_box->max_range_gate = ceil((sin((dist_s + radius_stratiform)/kea_and_radar)*kea_and_radar/cos(polar_box->other_angle)) / range_resolution);

    polar_box->min_angle = floor((angle - del_angle) / angular_resolution);
    polar_box->max_angle = ceil((angle + del_angle) / angular_resolution);

    polar_box->num_ranges = ceil(polar_box->max_range_gate - polar_box->min_range_gate);
    polar_box->num_angles = ceil(polar_box->max_angle - polar_box->min_angle);

    free(centre);
    free(radar_point);

    return 0; // success
}

*/
/*
int fill_polar_box(Polar_box* polar_box, double time, const Spatial_raincell* s_raincell, const Radar* radar, const Raincell* raincell) {
    if (!polar_box) return -1;

    if (strcmp(get_scanning_mode(radar), "PPI") != 0) {
        printf("fill_polar_box:\nAt simulation time %.2lf you are trying to fill a polar box for a %s\nPolar box NOT updated.\n\n", time, get_scanning_mode(radar));
        return -1;
    }

    // Earth 4/3 mean earth model
    double kea_and_radar = KEA + radar->z;

    Point* centre = get_position_raincell(time, s_raincell);
    Point* radar_point = get_position_radar(radar);

    double diff_x = centre->x - radar_point->x;
    double diff_y = centre->y - radar_point->y;
    double dist_s = sqrt(diff_x * diff_x + diff_y * diff_y);
    double dist = sin(dist_s / kea_and_radar) * kea_and_radar / cos(polar_box->other_angle);
    double radius_stratiform = raincell_get_radius_stratiform(raincell);

    polar_box->range_resolution = get_range_res_radar(radar);
    double range_resolution = polar_box->range_resolution;

    double angle = atan2(diff_y, diff_x);
    if (angle < 0) angle += 2 * M_PI;
    angle = angle * RAD2DEG;

    double del_angle = atan2(radius_stratiform, dist) * RAD2DEG;

    polar_box->angular_resolution = get_angular_res_radar(radar);
    double angular_resolution = polar_box->angular_resolution;

    polar_box->radar_id = get_radar_id(radar);

    polar_box->min_range_gate = floor((sin((dist_s - radius_stratiform) / kea_and_radar) * kea_and_radar / cos(polar_box->other_angle)) / range_resolution);
    polar_box->max_range_gate = ceil((sin((dist_s + radius_stratiform) / kea_and_radar) * kea_and_radar / cos(polar_box->other_angle)) / range_resolution);

    polar_box->min_angle = floor((angle - del_angle) / angular_resolution);
    polar_box->max_angle = ceil((angle + del_angle) / angular_resolution);

    // Ensure at least 1 range and 1 angle
    polar_box->num_ranges = (double)fmax(1, ceil(polar_box->max_range_gate - polar_box->min_range_gate));
    polar_box->num_angles = (double)fmax(1, ceil(polar_box->max_angle - polar_box->min_angle));

    free(centre);
    free(radar_point);

    return 0; // success
}
*/
/*
// ---------------------- Fill Polar Box --------CONSTANT SIZES!!!! WRONG--------------
int fill_polar_box(Polar_box* polar_box, double time, const Spatial_raincell* s_raincell, const Radar* radar, const Raincell* raincell) {
    if (!polar_box || !radar || !raincell || !s_raincell) return -1;

    if (strcmp(get_scanning_mode(radar), "PPI") != 0) {
        printf("fill_polar_box: Not a PPI scan at time %.2lf\n", time);
        return -1;
    }

    double kea_and_radar = KEA + radar->z;

    Point* centre = get_position_raincell(time, s_raincell);
    Point* radar_point = get_position_radar(radar);

    double diff_x = centre->x - radar_point->x;
    double diff_y = centre->y - radar_point->y;
    double dist_s = sqrt(diff_x * diff_x + diff_y * diff_y);
    double dist = sin(dist_s / kea_and_radar) * kea_and_radar / cos(polar_box->other_angle);
    double radius_stratiform = raincell_get_radius_stratiform(raincell);

    double angle = atan2(diff_y, diff_x);
    if (angle < 0) angle += 2 * M_PI;
    angle = angle * RAD2DEG;

    double del_angle = atan2(radius_stratiform, dist) * RAD2DEG;

    polar_box->range_resolution = get_range_res_radar(radar);
    polar_box->angular_resolution = get_angular_res_radar(radar);
    polar_box->radar_id = get_radar_id(radar);

    // Compute min/max range gates and angles
    int min_range_idx = (int)floor((dist_s - radius_stratiform) / polar_box->range_resolution);
    int max_range_idx = (int)ceil((dist_s + radius_stratiform) / polar_box->range_resolution);
    int min_angle_idx = (int)floor((angle - del_angle) / polar_box->angular_resolution);
    int max_angle_idx = (int)ceil((angle + del_angle) / polar_box->angular_resolution);

    // Clip to safe non-negative bounds
    if (min_range_idx < 0) min_range_idx = 0;
    if (min_angle_idx < 0) min_angle_idx = 0;
    if (max_range_idx <= min_range_idx) max_range_idx = min_range_idx + 1;
    if (max_angle_idx <= min_angle_idx) max_angle_idx = min_angle_idx + 1;

    polar_box->min_range_gate = min_range_idx;
    polar_box->max_range_gate = max_range_idx;
    polar_box->min_angle = min_angle_idx;
    polar_box->max_angle = max_angle_idx;

    polar_box->num_ranges = max_range_idx - min_range_idx;
    polar_box->num_angles = max_angle_idx - min_angle_idx;

    free(centre);
    free(radar_point);

    return 0;
}
*/

int fill_polar_box(Polar_box* polar_box, double time,
                   const Spatial_raincell* s_raincell,
                   const Radar* radar, const Raincell* raincell) {
    if (!polar_box || !radar || !s_raincell || !raincell) return -1;

    if (strcmp(get_scanning_mode(radar), "PPI") != 0) {
        printf("fill_polar_box: Unsupported scan mode %s\n", get_scanning_mode(radar));
        return -1;
    }

    double kea_and_radar = KEA + radar->z;

    Point* centre = get_position_raincell(time, s_raincell);//time in seconds.
    Point* radar_point = get_position_radar(radar);

    double diff_x = centre->x - radar_point->x;
    double diff_y = centre->y - radar_point->y;
    double dist_s = sqrt(diff_x * diff_x + diff_y * diff_y);
    double dist = sin(dist_s / kea_and_radar) * kea_and_radar / cos(polar_box->other_angle);
    double radius_stratiform = raincell_get_radius_stratiform(raincell);

    polar_box->range_resolution = get_range_res_radar(radar);
    polar_box->angular_resolution = get_angular_res_radar(radar);

    double angle = atan2(diff_y, diff_x);
    if (angle < 0) angle += 2 * M_PI;
    angle = angle * RAD2DEG;

    double del_angle = atan2(radius_stratiform, dist) * RAD2DEG;

    polar_box->radar_id = get_radar_id(radar);

    // Compute min/max gates and angles
    polar_box->min_range_gate = floor((sin((dist_s - radius_stratiform)/kea_and_radar)*kea_and_radar/cos(polar_box->other_angle)) / polar_box->range_resolution);
    polar_box->max_range_gate = ceil((sin((dist_s + radius_stratiform)/kea_and_radar)*kea_and_radar/cos(polar_box->other_angle)) / polar_box->range_resolution);

    polar_box->min_angle = floor((angle - del_angle) / polar_box->angular_resolution);
    polar_box->max_angle = ceil((angle + del_angle) / polar_box->angular_resolution);

    // Dynamically compute sizes
    int num_ranges = (int)(polar_box->max_range_gate - polar_box->min_range_gate + 1);
    int num_angles = (int)(polar_box->max_angle - polar_box->min_angle + 1);

    // Only reallocate if size changed or not allocated yet
    if (!polar_box->grid || (int)polar_box->num_ranges != num_ranges || (int)polar_box->num_angles != num_angles) {
        free(polar_box->grid);
        free(polar_box->height_grid);

        polar_box->grid = malloc(sizeof(double) * num_ranges * num_angles);
        polar_box->height_grid = malloc(sizeof(double) * num_ranges * num_angles);
        if (!polar_box->grid || !polar_box->height_grid) {
            perror("Failed to allocate polar box grids");
            free(centre);
            free(radar_point);
            return -1;
        }
    }

	


    polar_box->num_ranges = num_ranges;
    polar_box->num_angles = num_angles;

    free(centre);
    free(radar_point);

    return 0;
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

double get_range_res_polar_box(const struct Polar_box* box) {
	return box->range_resolution;
}

double get_angular_res_polar_box(const struct Polar_box* box){
	return box->angular_resolution;
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

	// Function to find Radar by ID ONLY BY ID!!              
const Radar* find_radar_by_id_ONLY(int idA) {
    for (int i = 0; i < radar_count; ++i) {   
        if (radar_list[i]->id == idA) {
            return radar_list[i];
        }
    }
    printf("find_radar_by_id_ONLY()\nRadar with id %d was not found. found_radar is allocated to NULL\n\n", idA);
    return NULL; // Not found

}



/*
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

}*/

void print_polar_box(const Polar_box* box) {
    if (box == NULL) {
        printf("polar box not found (points to NULL) in print_polar_grid function.\n\n");
        return;
    }

    // Print polar box info directly from the box
    printf("Radar ID      : %d\n", box->radar_id);
    printf("Num of Angles : %.0f\n", box->num_angles);
    printf("Num of Ranges : %.0f\n", box->num_ranges);
    printf("Between angles: [%.2f, %.2f]\n", box->min_angle, box->max_angle);
    printf("Between ranges: [%.2f, %.2f]\n",
           box->min_range_gate * box->range_resolution,
           box->max_range_gate * box->range_resolution);

	printf("the four values: {%.2lf,\n 	%.2lf,\n	%.2lf,\n	%.2lf}\n\n\n", box->min_range_gate * box->range_resolution*cos(box->min_angle*DEG2RAD),box->min_range_gate * box->range_resolution*sin(box->min_angle*DEG2RAD),box->max_range_gate * box->range_resolution*cos(box->min_angle*DEG2RAD),box->max_range_gate * box->range_resolution*sin(box->min_angle*DEG2RAD));


    printf("\n");
}


Bounding_box* create_bounding_box_for_polar_box(const Polar_box* p_box, const Radar** radars, int num_radars){
if(p_box==NULL){printf("create_bounding+box_for_polar_plot\n You are trying to create a bounding box for a polar box which is not defined (points to NULL)\n The bounding box will be assigned NULL\n\n");return NULL;}
	
	const Radar* found_radar = find_radar_by_id(p_box, radars, num_radars);

double rmin = get_min_range_gate(p_box) * get_range_res_radar(found_radar);
double curvature_correction_min = cos(p_box->other_angle + atan2(rmin*cos(p_box->other_angle),(KEA+rmin*sin(p_box->other_angle))));
double rmax = get_max_range_gate(p_box) * get_range_res_radar(found_radar);
double curvature_correction_max = cos(p_box->other_angle + atan2(rmax*cos(p_box->other_angle),(KEA+rmax*sin(p_box->other_angle))));
double anglemin = get_min_angle(p_box) * DEG2RAD;
double anglemax = get_max_angle(p_box) * DEG2RAD;
double anglemid = (anglemin + anglemax) / 2;

double xs[5] = {
    rmax * curvature_correction_max * cos(anglemin),
    rmin * curvature_correction_min * cos(anglemin),
    rmax * curvature_correction_max * cos(anglemax),
    rmin * curvature_correction_min * cos(anglemax),
    rmax * curvature_correction_max * cos(anglemid)
};

double ys[5] = {
    rmax * curvature_correction_max * sin(anglemin),
    rmin * curvature_correction_min * sin(anglemin),
    rmax * curvature_correction_max * sin(anglemax),
    rmin * curvature_correction_min * sin(anglemax),
    rmax * curvature_correction_max * sin(anglemid)
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


Bounding_box* create_bounding_box_for_polar_box_EZ(const Polar_box* p_box) {
    if (p_box == NULL) {
        printf("create_bounding_box_for_polar_box_GLOBAL()\n"
               "You are trying to create a bounding box for a polar box which is not defined (points to NULL).\n"
               "The bounding box will be assigned NULL.\n\n");
        return NULL;
    }

    // Use global radar registry
    const Radar* found_radar = find_radar_by_id_ONLY(p_box->radar_id);
    if (found_radar == NULL) {
        printf("create_bounding_box_for_polar_box_GLOBAL()\n"
               "No radar found with id %d. Returning NULL.\n\n", p_box->radar_id);
        return NULL;
    }

    double rmin = get_min_range_gate(p_box) * get_range_res_radar(found_radar);
    double curvature_correction_min = cos(
        p_box->other_angle + atan2(rmin * cos(p_box->other_angle),
                                   (KEA + rmin * sin(p_box->other_angle)))
    );

    double rmax = get_max_range_gate(p_box) * get_range_res_radar(found_radar);
    double curvature_correction_max = cos(
        p_box->other_angle + atan2(rmax * cos(p_box->other_angle),
                                   (KEA + rmax * sin(p_box->other_angle)))
    );

    double anglemin = get_min_angle(p_box) * DEG2RAD;
    double anglemax = get_max_angle(p_box) * DEG2RAD;
    double anglemid = (anglemin + anglemax) / 2;

    double xs[5] = {
        rmax * curvature_correction_max * cos(anglemin),
        rmin * curvature_correction_min * cos(anglemin),
        rmax * curvature_correction_max * cos(anglemax),
        rmin * curvature_correction_min * cos(anglemax),
        rmax * curvature_correction_max * cos(anglemid)
    };

    double ys[5] = {
        rmax * curvature_correction_max * sin(anglemin),
        rmin * curvature_correction_min * sin(anglemin),
        rmax * curvature_correction_max * sin(anglemax),
        rmin * curvature_correction_min * sin(anglemax),
        rmax * curvature_correction_max * sin(anglemid)
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
    if (!bbox) {
        printf("Memory allocation failed for Bounding_box.\n");
        return NULL;
    }

    bbox->topLeft.x     = xmin + pos_radar->x;
    bbox->topLeft.y     = ymax + pos_radar->y;
    bbox->topRight.x    = xmax + pos_radar->x;
    bbox->topRight.y    = ymax + pos_radar->y;
    bbox->bottomLeft.x  = xmin + pos_radar->x;
    bbox->bottomLeft.y  = ymin + pos_radar->y;
    bbox->bottomRight.x = xmax + pos_radar->x;
    bbox->bottomRight.y = ymin + pos_radar->y;

    return bbox;
}




double calculate_height_of_beam_at_range(double range, double elevation, double height_of_radar){
	double height_from_earth_centre = (KEA+height_of_radar);
	double corrected_elevation = elevation + atan2(range*cos(elevation), KEA+range*sin(elevation));
	return sqrt(range*range + height_from_earth_centre*height_from_earth_centre + 2*range*height_from_earth_centre*sin(corrected_elevation))-KEA;
}


int sample_from_relative_location_in_raincell(double range, double angle, double elevation, const Point* radar_centre, const Point* spatial_centre, const Raincell* raincell) {

	
    double curvature_correction = cos(elevation + atan2(range*cos(elevation),(KEA+range*sin(elevation))));
    // 1. Convert polar to cartesian in radar coordinates
    double dx = range * curvature_correction * cos(angle*DEG2RAD);
    double dy = range * curvature_correction * sin(angle*DEG2RAD);

    // 2. Absolute coordinates of the radar sample point
    double sample_x = radar_centre->x + dx;
    double sample_y = radar_centre->y + dy;

    // 3. Compute coordinates relative to raincell center
    double rel_x = sample_x - spatial_centre->x;
    double rel_y = sample_y - spatial_centre->y;

    // 4. Apply core offset in the direction of movement (assume offset along x-axis for simplicity)
    double core_centre_x = raincell_get_offset_centre_core(raincell);
    double core_rel_x = rel_x + core_centre_x;

    // 5. Compute distances
    double distance_to_centre = sqrt(rel_x * rel_x + rel_y * rel_y);
    double distance_to_core = sqrt(core_rel_x * core_rel_x + rel_y * rel_y);

    // 6. Determine which region the sample lies in
    if (distance_to_centre > raincell_get_radius_stratiform(raincell)) {
        return 0; // Outside the raincell
    } else if (distance_to_core <= raincell_get_radius_core(raincell)) {
        return 2; // Inside core region
    } else {
        return 1; // Inside stratiform region
    }
}
void fill_polar_box_grid(Polar_box* box, const Radar* radar,
                         const Spatial_raincell* s_raincell, const Raincell* raincell,
                         double time) {
    if (!box || !radar || !s_raincell || !raincell) return;

    int num_ranges = (int)box->num_ranges;
    int num_angles = (int)box->num_angles;
// Ensure the struct matches actual allocated sizes
box->num_ranges = num_ranges;
box->num_angles = num_angles;
    Point* pos_radar = get_position_radar(radar);
    Point* pos_raincell = get_position_raincell(time, s_raincell);
    double h0 = get_height_of_radar(radar);

    for (int ri = 0; ri < num_ranges; ri++) {
        double r1 = (box->min_range_gate + ri + 0.5) * box->range_resolution;
        double sample_height = calculate_height_of_beam_at_range(r1, box->other_angle, h0);

        for (int ai = 0; ai < num_angles; ai++) {
            double a1 = (box->min_angle + ai + 0.5) * box->angular_resolution;
            int sample = sample_from_relative_location_in_raincell(r1, a1, box->other_angle, pos_radar, pos_raincell, raincell);

            // Flattened grid write
            box->grid[ri * num_angles + ai] = (double)sample;
            box->height_grid[ri * num_angles + ai] = sample_height;
        }
    }

    free(pos_radar);
    free(pos_raincell);
}


/*

// ---------------------- Fill Polar Box Grid -------------CONSTANT SIZES!!!!!!!!!!!! WRONG---------
void fill_polar_box_grid(Polar_box* box,
                         const Radar* radar,
                         const Spatial_raincell* s_raincell,
                         const Raincell* raincell,
                         double time)
{
    if (!box || !radar || !s_raincell || !raincell) return;

    int num_ranges = box->num_ranges;
    int num_angles = box->num_angles;

    if (!box->grid) {
        box->grid = malloc(sizeof(double) * num_ranges * num_angles);
        if (!box->grid) { perror("Failed to allocate polar box grid"); exit(EXIT_FAILURE); }
    }

    if (!box->height_grid) {
        box->height_grid = malloc(sizeof(double) * num_ranges * num_angles);
        if (!box->height_grid) { perror("Failed to allocate polar box height_grid"); exit(EXIT_FAILURE); }
    }

    Point* pos_radar = get_position_radar(radar);
    Point* pos_raincell = get_position_raincell(time, s_raincell);
    double h0 = get_height_of_radar(radar);

    for (int ri = 0; ri < num_ranges; ri++) {
        double r1 = (box->min_range_gate + ri + 0.5) * box->range_resolution;
        double sample_height = calculate_height_of_beam_at_range(r1, box->other_angle, h0);

        for (int ai = 0; ai < num_angles; ai++) {
            double a1 = (box->min_angle + ai + 0.5) * box->angular_resolution;
            int sample = sample_from_relative_location_in_raincell(r1, a1, box->other_angle, pos_radar, pos_raincell, raincell);

            box->grid[ri * num_angles + ai] = (double)sample;
            box->height_grid[ri * num_angles + ai] = sample_height;
        }
    }

    free(pos_radar);
    free(pos_raincell);
}
*/

/*
void fill_polar_box_grid(Polar_box* box,
                         const Radar* radar,
                         const Spatial_raincell* s_raincell,
                         const Raincell* raincell,
                         double time)
{
    if (!box) return;

    size_t num_ranges = (size_t)box->num_ranges;
    size_t num_angles = (size_t)box->num_angles;

    // Reallocate grid if needed
    if (!box->grid) {
        box->grid = (double*)malloc(sizeof(double) * num_ranges * num_angles);
        if (!box->grid) {
            perror("Failed to allocate memory for polar box grid");
            exit(EXIT_FAILURE);
        }
    }

    // Reallocate height_grid if needed
    if (!box->height_grid) {
        box->height_grid = (double*)malloc(sizeof(double) * num_ranges * num_angles);
        if (!box->height_grid) {
            perror("Failed to allocate memory for polar box height_grid");
            exit(EXIT_FAILURE);
        }
    }

    Point* pos_radar = get_position_radar(radar);
    Point* pos_raincell = get_position_raincell(time, s_raincell);
    double h0 = get_height_of_radar(radar);
    double r1, a1, sample;
    double sample_height;

    for (size_t ri = 0; ri < num_ranges; ri++) {
        r1 = (box->min_range_gate + (double)ri + 0.5) * box->range_resolution;
        sample_height = calculate_height_of_beam_at_range(r1, box->other_angle, h0);

        for (size_t ai = 0; ai < num_angles; ai++) {
            a1 = (box->min_angle + (double)(ai + 0.5)) * box->angular_resolution;
            sample = (double)sample_from_relative_location_in_raincell(r1, a1, box->other_angle, pos_radar, pos_raincell, raincell);

            // Safe write to flattened grid
            size_t idx = ri * num_angles + ai;
            if (idx < num_ranges * num_angles) {
                box->grid[idx] = sample;
                box->height_grid[idx] = sample_height;
            }
        }
    }

    free(pos_radar);
    free(pos_raincell);
}
*/

/* // ADDRESSING MEMORY LEAK TRY 1
void fill_polar_box_grid(struct Polar_box* box,
                         const struct Radar* radar,
                         const struct Spatial_raincell* s_raincell,
                         const struct Raincell* raincell,
                         double time)
{
    assert(box != NULL);
    assert(radar != NULL);
    assert(s_raincell != NULL);
    assert(raincell != NULL);

    int num_ranges = (int)get_num_ranges(box);
    int num_angles = (int)get_num_angles(box);

    assert(num_ranges > 0 && num_angles > 0);

    size_t required_size = (size_t)num_ranges * (size_t)num_angles;

    // Reallocate grid if NULL or not large enough
    if (box->grid == NULL) {
        box->grid = (double *)malloc(sizeof(double) * required_size);
        if (!box->grid) {
            perror("Failed to allocate memory for polar box grid");
            exit(EXIT_FAILURE);
        }
    }

    if (box->height_grid == NULL) {
        box->height_grid = (double *)malloc(sizeof(double) * required_size);
        if (!box->height_grid) {
            perror("Failed to allocate memory for polar box height_grid");
            exit(EXIT_FAILURE);
        }
    }

    // Optional: reallocate if previously allocated size is smaller than required
    // This assumes you track allocated size in Polar_box (recommended)
    // if (box->allocated_grid_size < required_size) {
    //     free(box->grid);
    //     box->grid = malloc(sizeof(double) * required_size);
    //     free(box->height_grid);
    //     box->height_grid = malloc(sizeof(double) * required_size);
    //     box->allocated_grid_size = required_size;
    // }

    struct Point* pos_radar = get_position_radar(radar);
    struct Point* pos_raincell = get_position_raincell(time, s_raincell);
    double h0 = get_height_of_radar(radar);
    double r1, a1;
    double sample_height;
    int sample;
printf("num_ranges=%d num_angles=%d allocated_size=%zu\n",
       num_ranges, num_angles, box->allocated_grid_size);
    for (int ri = 0; ri < num_ranges; ri++) {
        r1 = (get_min_range_gate(box) + ri + 0.5) * get_range_res_polar_box(box);
        sample_height = calculate_height_of_beam_at_range(r1, box->other_angle, h0);

        for (int ai = 0; ai < num_angles; ai++) {
            a1 = (get_min_angle(box) + (ai + 0.5)) * get_angular_res_polar_box(box);
            sample = sample_from_relative_location_in_raincell(
                        r1, a1, box->other_angle, pos_radar, pos_raincell, raincell);

            // Flattened index
            size_t idx = (size_t)ri * (size_t)num_angles + (size_t)ai;
            assert(idx < required_size);

            box->grid[idx] = (double)sample;
            box->height_grid[idx] = sample_height;
        }
    }
}

*/



/*
void fill_polar_box_grid(struct Polar_box* box,
                         const struct Radar* radar,
                         const struct Spatial_raincell* s_raincell,
                         const struct Raincell* raincell,
                         double time)
{
    int num_ranges = (int)get_num_ranges(box);
    int num_angles = (int)get_num_angles(box);

    // Allocate memory if grid is NULL
    if (box->grid == NULL) {
        //box->grid = (double *)malloc(sizeof(double) * num_ranges * num_angles * 2);
        box->grid = (double *)malloc(sizeof(double) * num_ranges * num_angles);
        if (box->grid == NULL) {
            perror("Failed to allocate memory for polar box grid");
            exit(EXIT_FAILURE);
        }
    }

// Allocate memory if grid is NULL
if (box->height_grid == NULL) {
    //box->grid = (double *)malloc(sizeof(double) * num_ranges * num_angles * 2);
    box->height_grid = (double *)malloc(sizeof(double) * num_ranges * num_angles);
    if (box->height_grid == NULL) {
        perror("Failed to allocate memory for polar box heght_grid");
        exit(EXIT_FAILURE);
    }
}



    struct Point* pos_radar = get_position_radar(radar);
    struct Point* pos_raincell = get_position_raincell(time, s_raincell);
	double h0 = get_height_of_radar(radar);
    double r1;
    double a1;
	int sample;
	double sample_height;

    for (int ri = 0; ri < num_ranges; ri++) {
        r1 = (get_min_range_gate(box) + ri + 0.5) * get_range_res_polar_box(box);
	sample_height = calculate_height_of_beam_at_range(r1, box->other_angle, h0);
        for (int ai = 0; ai < num_angles; ai++) {
            a1 = (get_min_angle(box) + (ai + 0.5)) * get_angular_res_polar_box(box);
            sample = sample_from_relative_location_in_raincell(r1, a1,  box->other_angle, pos_radar, pos_raincell, raincell);

            // Store value in flattened grid
            //box->grid[ri * num_angles + ai*2 + 0] = sample_height;
            //box->grid[ri * num_angles + ai*2 + 1] = (double)sample;
            box->grid[ri * num_angles + ai] = (double)sample;
	    box->height_grid[ri*num_angles + ai] = sample_height;
	}
    }
}
*/



/*
void save_polar_box_grid_to_file(const Polar_box* box, const Radar* radar, int scan_index, const char* filename) {
    FILE* fp = fopen(filename, "a");  // Append mode to handle multiple scans
    if (!fp) {
        perror("Failed to open output file");
        exit(EXIT_FAILURE);
    }

    fprintf(fp, "=== BEGIN RADAR_SCAN ===\n");
    fprintf(fp, "scan.index=%d\n", scan_index);

    // Radar metadata
    fprintf(fp, "radar.id=%d\n", radar->id);
    fprintf(fp, "radar.frequency=%s\n", radar->frequency);
    fprintf(fp, "radar.scanning_mode=%s\n", radar->scanning_mode);
    fprintf(fp, "radar.x=%.3f\n", radar->x);
    fprintf(fp, "radar.y=%.3f\n", radar->y);
    fprintf(fp, "radar.z=%.3f\n", radar->z);
    fprintf(fp, "radar.maximum_range=%.3f\n", radar->maximum_range);
    fprintf(fp, "radar.range_resolution=%.3f\n", radar->range_resolution);
    fprintf(fp, "radar.angular_resolution=%.6f\n", radar->angular_resolution);

    // Polar box metadata
    fprintf(fp, "box.radar_id=%d\n", box->radar_id);
    fprintf(fp, "box.min_range_gate=%.3f\n", box->min_range_gate);
    fprintf(fp, "box.max_range_gate=%.3f\n", box->max_range_gate);
    fprintf(fp, "box.min_angle=%.6f\n", box->min_angle);
    fprintf(fp, "box.max_angle=%.6f\n", box->max_angle);
    fprintf(fp, "box.num_ranges=%.0f\n", box->num_ranges);
    fprintf(fp, "box.num_angles=%.0f\n", box->num_angles);
    fprintf(fp, "box.range_resolution=%.3f\n", box->range_resolution);
    fprintf(fp, "box.angular_resolution=%.6f\n", box->angular_resolution);

    // Grid data
    int total = (int)(box->num_ranges * box->num_angles);
    fprintf(fp, "grid.size=%d\n", total);
    fprintf(fp, "grid.data=");
    for (int i = 0; i < total; i++) {
        fprintf(fp, "%.2f", box->grid[i]);
        if (i < total - 1) {
            fprintf(fp, " ");
        }
    }
    fprintf(fp, "\n");

    fprintf(fp, "=== END RADAR_SCAN ===\n\n");

    fclose(fp);
}

*/


void save_polar_box_grid_to_file(const Polar_box* box, const Radar* radar, int scan_index, double scan_time,const char* filename) {
    FILE* fp = fopen(filename, "a");  // Append mode to handle multiple scans
    if (!fp) {
        perror("Failed to open output file");
        exit(EXIT_FAILURE);
    }  
    
    fprintf(fp, "=== BEGIN RADAR_SCAN ===\n");
    fprintf(fp, "scan.index=%d\n", scan_index);
    fprintf(fp,"scan.time=%lf\n", scan_time);
    // Radar metadata
    fprintf(fp, "radar.id=%d\n", radar->id);
    fprintf(fp, "radar.frequency=%s\n", radar->frequency);
    fprintf(fp, "radar.scanning_mode=%s\n", radar->scanning_mode);
    fprintf(fp, "radar.x=%.3f\n", radar->x);
    fprintf(fp, "radar.y=%.3f\n", radar->y);
    fprintf(fp, "radar.z=%.3f\n", radar->z);
    fprintf(fp, "radar.maximum_range=%.3f\n", radar->maximum_range);
    fprintf(fp, "radar.range_resolution=%.3f\n", radar->range_resolution);
    fprintf(fp, "radar.angular_resolution=%.6f\n", radar->angular_resolution);
    
    // Polar box metadata
    fprintf(fp, "box.radar_id=%d\n", box->radar_id);
    fprintf(fp, "box.min_range_gate=%.3f\n", box->min_range_gate);
    fprintf(fp, "box.max_range_gate=%.3f\n", box->max_range_gate);
    fprintf(fp, "box.min_angle=%.6f\n", box->min_angle);
    fprintf(fp, "box.max_angle=%.6f\n", box->max_angle);
    fprintf(fp, "box.num_ranges=%d\n", box->num_ranges);
    fprintf(fp, "box.num_angles=%d\n", box->num_angles);
    fprintf(fp, "box.range_resolution=%.3f\n", box->range_resolution);
    fprintf(fp, "box.angular_resolution=%.6f\n", box->angular_resolution);
    fprintf(fp, "box.other_angle=%.6f\n", box->other_angle); 
    // Grid data
    int total = (int)(box->num_ranges * box->num_angles);
    fprintf(fp, "grid.size=%d\n", total);
    fprintf(fp, "grid.data=");
    for (int i = 0; i < total; i++) {
        fprintf(fp, "%.2f", box->grid[i]);
        if (i < total - 1) {
            fprintf(fp, " ");
        }
    }
    fprintf(fp, "\n");

    // Height grid data
    if (box->height_grid != NULL) {
        fprintf(fp, "height.size=%d\n", total);
        fprintf(fp, "height.data=");
        for (int i = 0; i < total; i++) {
            fprintf(fp, "%.2f", box->height_grid[i]);
            if (i < total - 1) {
                fprintf(fp, " ");
            }
        }
        fprintf(fp, "\n");
    }

    fprintf(fp, "=== END RADAR_SCAN ===\n\n");

    fclose(fp);
}




//creating a radar, a polar box, and a radarscan type from the radar_scans file. 

void read_radar_scans(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file");
        return;
    }

    char line[4096];
    int in_block = 0;

    // Temporary vars
    int scan_index = 0;
    double scan_time = 0.0;
    int radar_id = 0;
    char freq[2] = "", mode[4] = "";
    double x=0,y=0,z=0,max_range=0,range_res=0,angular_res=0;
    double min_gate=0, max_gate=0, min_angle=0, max_angle=0;
    double num_ranges=0, num_angles=0;
    int grid_size = 0;
    double* grid_data = NULL;
    double other_angle = 0;
    int height_size = 0;
    double* height_data = NULL;

    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, "=== BEGIN RADAR_SCAN ===")) {
            in_block = 1;
            // Reset vars
            scan_index = radar_id = 0;
	    scan_time = 0.0;
            freq[0] = '\0'; mode[0] = '\0';
            x = y = z = max_range = range_res = angular_res = 0;
            min_gate = max_gate = min_angle = max_angle = other_angle = 0;
            num_ranges = num_angles = 0;
            grid_size = 0;
            free(grid_data); grid_data = NULL;
	    height_size = 0;
	    free(height_data);height_data=NULL;
    	    continue;
        }

        if (strstr(line, "=== END RADAR_SCAN ===")) {
            in_block = 0;

            Radar* radar = get_or_create_radar(radar_id, freq, mode, x, y, z,
                                               max_range, range_res, angular_res);
            Polar_box* box = create_polar_box(radar_id, min_gate, max_gate,
                                              min_angle, max_angle, num_ranges,
                                              num_angles, range_res, angular_res,
                                              grid_size, other_angle, grid_data,height_size,height_data);

            radar_scans[scan_count].scan_index = scan_index;
	    radar_scans[scan_count].time = scan_time;
	    radar_scans[scan_count].radar = radar;
            radar_scans[scan_count].box = box;
            scan_count++;

            free(grid_data);
            grid_data = NULL;
            continue;
        }

        if (!in_block) continue;

        // Parse lines
        if (sscanf(line, "scan.index=%d", &scan_index)) continue;
        if (sscanf(line, "scan.time=%lf", &scan_time)) continue;
	if (sscanf(line, "radar.id=%d", &radar_id)) continue;
        if (sscanf(line, "radar.frequency=%1s", freq)) continue;
        if (sscanf(line, "radar.scanning_mode=%3s", mode)) continue;
        if (sscanf(line, "radar.x=%lf", &x)) continue;
        if (sscanf(line, "radar.y=%lf", &y)) continue;
        if (sscanf(line, "radar.z=%lf", &z)) continue;
        if (sscanf(line, "radar.maximum_range=%lf", &max_range)) continue;
        if (sscanf(line, "radar.range_resolution=%lf", &range_res)) continue;
        if (sscanf(line, "radar.angular_resolution=%lf", &angular_res)) continue;
        if (sscanf(line, "box.min_range_gate=%lf", &min_gate)) continue;
        if (sscanf(line, "box.max_range_gate=%lf", &max_gate)) continue;
        if (sscanf(line, "box.min_angle=%lf", &min_angle)) continue;
        if (sscanf(line, "box.max_angle=%lf", &max_angle)) continue;
        if (sscanf(line, "box.num_ranges=%lf", &num_ranges)) continue;
        if (sscanf(line, "box.num_angles=%lf", &num_angles)) continue;
        if (sscanf(line, "box.range_resolution=%lf", &range_res)) continue;
        if (sscanf(line, "box.angular_resolution=%lf", &angular_res)) continue;
        if (sscanf(line, "box.other_angle=%lf", &other_angle)) continue;

        if (sscanf(line, "grid.size=%d", &grid_size)) {
            if (grid_size > 0) {
                grid_data = (double*)malloc(sizeof(double) * grid_size);
                if (!grid_data) {
                    printf("Memory allocation failed for grid.\n");
                    fclose(file);
                    return;
                }
            }
            continue;
        }


	if (sscanf(line, "height.size=%d", &height_size)) {
	    if (height_size > 0) {
	        height_data = (double*)malloc(sizeof(double) * height_size);
	        if (!height_data) {
	            printf("Memory allocation failed for height.\n");
	            fclose(file);
	            return;
	        }
	    }
	    continue;
	}


        if (strstr(line, "grid.data=") && grid_size > 0 && grid_data != NULL) {
            // Skip "grid.data=" prefix
            char* p = strstr(line, "grid.data=") + strlen("grid.data=");
            int filled = 0;

            // First line of grid.data
            while (*p && filled < grid_size) {
                while (*p && isspace(*p)) p++;
                if (*p) {
                    double val;
                    if (sscanf(p, "%lf", &val) == 1) {
                        grid_data[filled++] = val;
                        while (*p && !isspace(*p)) p++;
                    }
                }
            }

            // Continue reading lines if not full
            while (filled < grid_size && fgets(line, sizeof(line), file)) {
                p = line;
                while (*p && filled < grid_size) {
                    while (*p && isspace(*p)) p++;
                    if (*p) {
                        double val;
                        if (sscanf(p, "%lf", &val) == 1) {
                            grid_data[filled++] = val;
                            while (*p && !isspace(*p)) p++;
                        }
                    }
                }
            }
        }

if (strstr(line, "height.data=") && height_size > 0 && height_data != NULL) {
    char* p = strstr(line, "height.data=") + strlen("height.data=");
    int filled = 0;

    while (*p && filled < height_size) {
        while (*p && isspace(*p)) p++;
        if (*p) {
            double val;
            if (sscanf(p, "%lf", &val) == 1) {
                height_data[filled++] = val;
                while (*p && !isspace(*p)) p++;
            }
        }
    }

    while (filled < height_size && fgets(line, sizeof(line), file)) {
        p = line;
        while (*p && filled < height_size) {
            while (*p && isspace(*p)) p++;
            if (*p) {
                double val;
                if (sscanf(p, "%lf", &val) == 1) {
                    height_data[filled++] = val;
                    while (*p && !isspace(*p)) p++;
                }
            }
        }
    }
}
    }

    fclose(file);
}



Bounding_box* bounding_box_from_textfile(const Polar_box* p_box, const Radar* radar){
if(p_box==NULL){printf("create_bounding+box_for_polar_plot\n You are trying to create a bounding box for a polar box which is not defined (points to NULL)\n The bounding box will be assigned NULL\n\n");return NULL;}	
 



//double rmin = p_box->min_range_gate * p_box->range_resolution;
double rmin = (p_box->min_range_gate * p_box->range_resolution);
double curvature_correction_min = cos(p_box->other_angle + atan2(rmin*cos(p_box->other_angle),(KEA+rmin*sin(p_box->other_angle))));
//double rmax = p_box->max_range_gate * p_box->range_resolution;
double rmax = p_box->max_range_gate * p_box->range_resolution;
double curvature_correction_max = cos(p_box->other_angle + atan2(rmax*cos(p_box->other_angle),(KEA+rmax*sin(p_box->other_angle))));
double anglemin = p_box->min_angle * DEG2RAD;
double anglemax = p_box->max_angle * DEG2RAD;
double anglemid = (anglemin + anglemax) / 2;
 
double xs[5] = {
    rmax * curvature_correction_max * cos(anglemin),
    rmin * curvature_correction_min * cos(anglemin),
    rmax * curvature_correction_max * cos(anglemax),
    rmin * curvature_correction_min * cos(anglemax),
    rmax * curvature_correction_max * cos(anglemid)
};
 
double ys[5] = {
    rmax * curvature_correction_max * sin(anglemin),
    rmin * curvature_correction_min * sin(anglemin),
    rmax * curvature_correction_max * sin(anglemax),
    rmin * curvature_correction_min * sin(anglemax),
    rmax * curvature_correction_max * sin(anglemid)

};
 
 
double xmin = xs[0], xmax = xs[0];
double ymin = ys[0], ymax = ys[0];
 
for (int i = 1; i < 5; ++i) {
    if (xs[i] < xmin) xmin = xs[i];
    if (xs[i] > xmax) xmax = xs[i];
    if (ys[i] < ymin) ymin = ys[i];
    if (ys[i] > ymax) ymax = ys[i];
}


                                   
Point* pos_radar = get_position_radar(radar);
                                   
                                   
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

void free_polar_box(Polar_box *box) {
    if (!box) return;  // Safety check

    if (box->grid) {
        free(box->grid);
        box->grid = NULL;
    }

    if (box->height_grid) {
        free(box->height_grid);
        box->height_grid = NULL;
    }

    if (box->attenuation_grid) {
        free(box->attenuation_grid);
        box->attenuation_grid = NULL;
    }

    free(box);  // Finally, free the struct itself
}

