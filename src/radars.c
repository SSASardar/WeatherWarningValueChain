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
 	double *grid;	
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


Polar_box* init_polar_box() {
    Polar_box* polar_box = malloc(sizeof(Polar_box));
    if (!polar_box) {
        fprintf(stderr, "Failed to allocate Polar_box\n");
        return NULL;
    }
    // Initialize pointers to NULL or zero out members as needed
    polar_box->grid = NULL;
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
    return polar_box;
}


int fill_polar_box(Polar_box* polar_box, double time, const Spatial_raincell* s_raincell, const Radar* radar, const Raincell* raincell) {
    if (!polar_box) return -1;

    if (strcmp(get_scanning_mode(radar), "PPI") != 0) {
        printf("fill_polar_box:\nAt simulation time %.2lf you are trying to fill a polar box for a %s\nPolar box NOT updated.\n\n", time, get_scanning_mode(radar));
        return -1;
    }

    Point* centre = get_position_raincell(time, s_raincell);
    Point* radar_point = get_position_radar(radar);

    double diff_x = centre->x - radar_point->x;
    double diff_y = centre->y - radar_point->y;
    double dist = sqrt(diff_x * diff_x + diff_y * diff_y);
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
    polar_box->min_range_gate = floor((dist - radius_stratiform) / range_resolution);
    polar_box->max_range_gate = ceil((dist + radius_stratiform) / range_resolution);

    polar_box->min_angle = floor((angle - del_angle) / angular_resolution);
    polar_box->max_angle = ceil((angle + del_angle) / angular_resolution);

    polar_box->num_ranges = polar_box->max_range_gate - polar_box->min_range_gate;
    polar_box->num_angles = polar_box->max_angle - polar_box->min_angle;

    free(centre);
    free(radar_point);

    return 0; // success
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

double calculate_height_of_beam_at_range(double range, double elevation, double mean_earth_radius, double height_of_radar){
	double height_from_earth_centre = (1.333333*mean_earth_radius+height_of_radar);
	return sqrt(range*range + height_from_earth_centre*height_from_earth_centre + 2*range*height_from_earth_centre*sin(elevation))-1.33333*mean_earth_radius;
}


int sample_from_relative_location_in_raincell(double range, double angle, const Point* radar_centre, const Point* spatial_centre, const Raincell* raincell) {
    // 1. Convert polar to cartesian in radar coordinates
    double dx = range * cos(angle*DEG2RAD);
    double dy = range * sin(angle*DEG2RAD);

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

    struct Point* pos_radar = get_position_radar(radar);
    struct Point* pos_raincell = get_position_raincell(time, s_raincell);
	double h0 = get_height_of_radar(radar);
    double r1;
    double a1;
	int sample;
	//double sample_height;

    for (int ri = 0; ri < num_ranges; ri++) {
        r1 = (get_min_range_gate(box) + ri + 0.5) * get_range_res_polar_box(box);
	//sample_height = calculate_height_of_beam_at_range(r1, 0.0, 6371000,h0);
        for (int ai = 0; ai < num_angles; ai++) {
            a1 = (get_max_angle(box) - (ai + 0.5)) * get_angular_res_polar_box(box);
            sample = sample_from_relative_location_in_raincell(r1, a1, pos_radar, pos_raincell, raincell);

            // Store value in flattened grid
            //box->grid[ri * num_angles + ai*2 + 0] = sample_height;
            //box->grid[ri * num_angles + ai*2 + 1] = (double)sample;
            box->grid[ri * num_angles + ai] = (double)sample;
        }
    }
}


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








