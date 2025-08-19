


#ifndef PROCESSING_H
#define PROCESSING_H

#include<stdbool.h>
#include"radars.h"
#include "common.h"
#include <stdio.h>
#include "vertical_profiles.h"


typedef struct Cart_grid {
	double resolution;
	double *grid;
	double *height_grid;
	double *attenuation_grid;
	int num_elements;
	int num_x;
	int num_y;
	Point ref_point;
} Cart_grid;


typedef struct Vol_scan {
    int num_PPIs;              // number of PPI scans
    int num_elements;          // total number of elements in grid
    int num_x;                 // grid size in X
    int num_y;                 // grid size in Y
    Point ref_point;           // minimum reference point of all Cart_grids
    VPR_point *grid;           // main grid, size = num_elements * num_PPIs
    VPR_point *display_grid;   // display grid, regularized resolution
} Vol_scan;






Cart_grid* Cart_grid_init(double resolution, int num_x, int num_y, Point ref_point);

double normalizeAngle(double angle); // angle in radians.

bool isAngleBetween(double angle, double minAngle, double maxAngle);


bool isPointInSectorAnnulus(Point p, Point center, double minAngle, double maxAngle, double minRange, double maxRange); 

bool getPolarBoxIndex(Point p, double c_x, double c_y,const Polar_box* box, int *range_idx, int *angle_idx);


void writeCartGridToFile(Cart_grid* cg, int scan_id, int what_to_print); 

Vol_scan *init_vol_scan(Cart_grid *cart_grids, int num_PPIs);

static inline int vol_index(const Vol_scan *vol, int x, int y, int ppi) {
    return ppi * vol->num_elements + y * vol->num_x + x;
}

int add_cart_grid_to_volscan(Vol_scan *vol, Cart_grid *grid, int ppi_index);

void free_vol_scan(Vol_scan *vol);


int write_vol_scan_ppi_to_file(const Vol_scan *vol, int ppi_index, const char *filename);



#endif /* PROCESSING_H  */
