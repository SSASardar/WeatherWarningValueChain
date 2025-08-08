// this is the processing.c file to implement processing of the radar data. 
//
//


// include statements:
#include "processing.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
//#include "radars.h"
#include "spatial_coords_raincell.h"
#include "material_coords_raincell.h"
#include "common.h"
#include <ctype.h>




Cart_grid* Cart_grid_init(double resolution, int num_x, int num_y, Point ref_point) {
    Cart_grid *cg = (Cart_grid *)malloc(sizeof(Cart_grid));
    if (!cg) return NULL;

    cg->resolution = resolution;
    cg->num_x = num_x;
    cg->num_y = num_y;
    cg->num_elements = num_x * num_y;
    cg->ref_point = ref_point;

    cg->grid = (double *)malloc(sizeof(double) * cg->num_elements);
    cg->height_grid = (double *)malloc(sizeof(double)*cg->num_elements);
    cg->attenuation_grid = (double *)malloc(sizeof(double)*3*cg->num_elements);
    
    if (!cg->grid) {
        free(cg);
	printf("Allocating grid in Cart_grid_init() failed! allocating NULL!! \n");
        return NULL;
    }

    for (int i = 0; i < cg->num_elements; i++) {
        cg->grid[i] = 0.0;
    	cg->height_grid[i] = 0.0;
    	cg->attenuation_grid[3*i+0]=0.0;
    	cg->attenuation_grid[3*i+1]=0.0;
    	cg->attenuation_grid[3*i+2]=0.0;
    }
	printf("success I think? \n");
    return cg;
}

// Normalize angle to [0, 2*PI)
double normalizeAngle(double angle) {
    while (angle < 0) angle += 2 * M_PI;
    while (angle >= 2 * M_PI) angle -= 2 * M_PI;
    return angle;
}

// Check if angle is between minAngle and maxAngle, considering wrapping
bool isAngleBetween(double angle, double minAngle, double maxAngle) {
    angle = normalizeAngle(angle);
    minAngle = normalizeAngle(minAngle);
    maxAngle = normalizeAngle(maxAngle);

    if (minAngle <= maxAngle) {
        return angle >= minAngle && angle <= maxAngle;
    } else {
        // Sector crosses the 0 angle line
        return angle >= minAngle || angle <= maxAngle;
    }
}

// Check if a point is inside the sector annulus with arbitrary center
bool isPointInSectorAnnulus(Point p, Point center, double minAngle, double maxAngle, double minRange, double maxRange) {
    // Translate the point relative to the center
    double dx = p.x - center.x;
    double dy = p.y - center.y;

    // Compute polar coordinates relative to the center
    double r = sqrt(dx * dx + dy * dy);
    if (r < minRange || r > maxRange) {
        return false;
    }

    double angle = atan2(dy, dx);     // Angle relative to center
    angle = normalizeAngle(angle);    // Convert to [0, 2*PI)

    return isAngleBetween(angle, minAngle, maxAngle);
}

/*
bool getPolarBoxIndex(Point p, double c_x, double c_y, const Polar_box* box, int *range_idx, int *angle_idx) {
    double dx = p.x - c_x;
    double dy = p.y - c_y;

    double range_res = box->range_resolution;
    double r = sqrt(dx * dx + dy * dy);
    if (r < box->min_range_gate*range_res || r > box->max_range_gate*range_res) return false;

    double angle = normalizeAngle(atan2(dy, dx));
    if (!isAngleBetween(angle, box->min_angle*DEG2RAD, box->max_angle*DEG2RAD)) return false;

    *range_idx = (int)floor((r - (double)box->min_range_gate*range_res) / box->range_resolution);
    double angle_diff = normalizeAngle(angle - box->min_angle);
    *angle_idx = (int)floor(angle_diff / box->angular_resolution*DEG2RAD);

    if (*range_idx >= (int)box->num_ranges || *angle_idx >= (int)box->num_angles)
        return false;

    return true;
}
*/

bool getPolarBoxIndex(Point p, double c_x, double c_y, const Polar_box* box, int *range_idx, int *angle_idx) {
    double dx = p.x - c_x;
    double dy = p.y - c_y;

    double r = sqrt(dx * dx + dy * dy);

    if (r < box->min_range_gate * box->range_resolution || r > box->max_range_gate * box->range_resolution)
        return false;

    double angle = normalizeAngle(atan2(dy, dx));  // radians

    if (!isAngleBetween(angle, box->min_angle * DEG2RAD, box->max_angle * DEG2RAD))
        return false;

    // Range index
    *range_idx = (int)((r - box->min_range_gate * box->range_resolution) / box->range_resolution);

    // Corrected: use radians for angle_diff
    double angle_diff = normalizeAngle(angle - box->min_angle * DEG2RAD);
    *angle_idx = (int)(angle_diff / (box->angular_resolution * DEG2RAD));

    if (*range_idx >= (int)box->num_ranges || *angle_idx >= (int)box->num_angles)
        return false;

    return true;
}

void writeCartGridToFile(Cart_grid* cg, int scan_id) {
    if (!cg || !cg->grid) {
        printf("Error: Null Cart_grid or grid.\n");
        return;
    }
char filename[100];
snprintf(filename, sizeof(filename), "outputs/cartesian_grid_output_%d.txt", scan_id);

FILE *fp = fopen(filename, "w");
    if (!fp) {
        printf("Error: Unable to open file for writing.\n");
        return;
    }

        for (int y =0;y< cg->num_y-1; y++) {
    for (int x =0; x<cg->num_x; x++) {
            int index = x * cg->num_y + y;
//            fprintf(fp, "%.2f ", cg->grid[index]);  // format as needed
//            fprintf(fp, "%.2f ", cg->height_grid[index]);  // format as needed
    	    fprintf(fp,  "%.2f ", 0.00*cg->attenuation_grid[3*index+0]+1.0*cg->attenuation_grid[3*index+1]+2.0*cg->attenuation_grid[3*index+2]);    
    }
        fprintf(fp, "\n");  // newline after each row
    }

    fclose(fp);
    printf("Grid successfully written to cartesian_grid_output.txt\n");
}

/*
void generate_attenuation_grid(Cartesian_grid* cg) {
	double temp_var;
	for (int i = 0 ; i < cg->num_elements;i++){
		temp_var = cg->grid[i];
		if (temp_var == 0.0) cg->attenuation_grid[3*i+0]
	}





}
*/
