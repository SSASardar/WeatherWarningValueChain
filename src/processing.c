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
#include <float.h>



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

void writeCartGridToFile(Cart_grid* cg, int scan_id, int what_to_print) {
    if (!cg || !cg->grid) {
        printf("Error: Null Cart_grid or grid.\n");
        return;
    }
char filename[100];

//snprintf(filename, sizeof(filename), "outputs/cartesian_grid_output_%d.txt", scan_id);
if (what_to_print == 0) snprintf(filename, sizeof(filename), "outputs/m_cartesian_grid_output_%d.txt", scan_id);

if (what_to_print == 1) snprintf(filename, sizeof(filename), "outputs/h_cartesian_grid_output_%d.txt", scan_id);

if (what_to_print == 2) snprintf(filename, sizeof(filename), "outputs/a_cartesian_grid_output_%d.txt", scan_id);





FILE *fp = fopen(filename, "w");
    if (!fp) {
        printf("Error: Unable to open file for writing.\n");
        return;
    }

        for (int y =0;y< cg->num_y-1; y++) {
    for (int x =0; x<cg->num_x; x++) {
            int index = x * cg->num_y + y;
	    if (what_to_print == 0) fprintf(fp, "%.2f ", cg->grid[index]);  // format as needed
            if (what_to_print == 1) fprintf(fp, "%.2f ", cg->height_grid[index]);  // format as needed
    	    if (what_to_print == 2) fprintf(fp,  "%.2f ", 0.00*cg->attenuation_grid[3*index+0]+1.0*cg->attenuation_grid[3*index+1]+2.0*cg->attenuation_grid[3*index+2]);    
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

Vol_scan *init_vol_scan(Cart_grid *cart_grids, int num_PPIs) {
    if (!cart_grids || num_PPIs <= 0) return NULL;

    Vol_scan *vol = (Vol_scan *)malloc(sizeof(Vol_scan));
    if (!vol) return NULL;

    vol->num_PPIs = num_PPIs;
    vol->num_x = 0;
    vol->num_y = 0;
    vol->ref_point.x = DBL_MAX;
    vol->ref_point.y = DBL_MAX;

    // Determine ref_point (minimum), num_x/num_y (maximum)

double min_x = DBL_MAX, min_y = DBL_MAX;
double max_x = -DBL_MAX, max_y = -DBL_MAX;

for (int i = 0; i < num_PPIs; i++) {
    Cart_grid *g = &cart_grids[i];

    if (g->ref_point.x < min_x) min_x = g->ref_point.x;
    if (g->ref_point.y < min_y) min_y = g->ref_point.y;

    double g_max_x = g->ref_point.x + g->resolution * (g->num_x - 1);
    double g_max_y = g->ref_point.y + g->resolution * (g->num_y - 1);

    if (g_max_x > max_x) max_x = g_max_x;
    if (g_max_y > max_y) max_y = g_max_y;
}



vol->ref_point.x = min_x;
vol->ref_point.y = min_y;

// Compute number of points
vol->num_x = (int)ceil((max_x - min_x) / cart_grids[0].resolution) + 1;
vol->num_y = (int)ceil((max_y - min_y) / cart_grids[0].resolution) + 1;
vol->num_elements = vol->num_x * vol->num_y;





/*
    double max_x = -DBL_MAX, max_y = -DBL_MAX;
for (int i = 0; i < num_PPIs; i++) {
    double grid_max_x = cart_grids[i].ref_point.x + cart_grids[i].num_x * cart_grids[i].resolution;
    double grid_max_y = cart_grids[i].ref_point.y + cart_grids[i].num_y * cart_grids[i].resolution;
    if (grid_max_x > max_x) max_x = grid_max_x;
    if (grid_max_y > max_y) max_y = grid_max_y;
}

double resolution = cart_grids[0].resolution;
vol->num_x = (int)ceil((max_x - vol->ref_point.x) / resolution);
vol->num_y = (int)ceil((max_y - vol->ref_point.y) / resolution);
vol->num_elements = vol->num_x * vol->num_y;
  */







// allocate grid: one VPR_point per PPI per cartesian point
    vol->grid = (VPR_point *)calloc(vol->num_elements * num_PPIs, sizeof(VPR_point));
    if (!vol->grid) {
        free(vol);
        return NULL;
    }

    // allocate display grid (same size as num_elements for simplicity)
    vol->display_grid = (VPR_point *)calloc(vol->num_elements, sizeof(VPR_point));
    if (!vol->display_grid) {
        free(vol->grid);
        free(vol);
        return NULL;
    }

    return vol;
}


int add_cart_grid_to_volscan(Vol_scan *vol, Cart_grid *grid, int ppi_index) {
    if (!vol || !grid) return -1;
    if (ppi_index < 0 || ppi_index >= vol->num_PPIs) return -2;

    for (int y = 0; y < grid->num_y; y++) {
        for (int x = 0; x < grid->num_x; x++) {
            int local_idx = y * grid->num_x + x;

            // Compute absolute coordinates relative to grid->ref_point
            double abs_x = grid->ref_point.x + x * grid->resolution;
            double abs_y = grid->ref_point.y + y * grid->resolution;

            // Compute indices in the Vol_scan grid
            int global_x = (int)floor((abs_x - vol->ref_point.x) / grid->resolution);
            int global_y = (int)floor((abs_y - vol->ref_point.y) / grid->resolution);

            if (global_x < 0 || global_x >= vol->num_x ||
                global_y < 0 || global_y >= vol->num_y) {
                continue; // outside bounds of volume scan
            }

            int vol_idx = vol_index(vol, global_x, global_y, ppi_index);

            // Fill VPR_point (set missing values to NaN if needed)
            VPR_point *pt = &vol->grid[vol_idx];
            pt->reflectivity = grid->grid ? grid->grid[local_idx] : NAN;
            pt->height       = grid->height_grid ? grid->height_grid[local_idx] : NAN;
        }
    }

    return 0;
}


void free_vol_scan(Vol_scan *vol) {
    if (!vol) return;
    if (vol->grid) free(vol->grid);
    if (vol->display_grid) free(vol->display_grid);
    free(vol);
}


int write_vol_scan_ppi_to_file(const Vol_scan *vol, int ppi_index, const char *filename) {
    if (!vol || !filename) return -1;
    if (ppi_index < 0 || ppi_index >= vol->num_PPIs) return -2;

    FILE *f = fopen(filename, "w");
    if (!f) return -3;

    fprintf(f, "# Vol_scan PPI slice %d\n", ppi_index);
    fprintf(f, "# Grid size: %d x %d\n", vol->num_x, vol->num_y);
    fprintf(f, "# Format: reflectivity height\n");

    for (int y = 0; y < vol->num_y; y++) {
        for (int x = 0; x < vol->num_x; x++) {
            int idx = vol_index(vol, x, y, ppi_index);
            double refl  = vol->grid[idx].reflectivity;
            double h     = vol->grid[idx].height;

            if (isnan(refl) || isnan(h)) {
                fprintf(f, "NaN NaN ");
            } else {
                fprintf(f, "%.2f %.2f ", refl, h);
            }
        }
        fprintf(f, "\n");
    }

    fclose(f);
    return 0;
}



















