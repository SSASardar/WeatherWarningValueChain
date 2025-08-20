


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
    int num_PPIs;
    int num_elements;
    int num_x;
    int num_y;
    Point ref_point;
    double resolution;

    double *grid_refl;      // size = num_elements * num_PPIs
    double *grid_height;    // optional
    double *grid_att;       // optional
    double *display_grid;   // optional, size = num_elements
} Vol_scan;




Cart_grid* Cart_grid_init(double resolution, int num_x, int num_y, Point ref_point);

double normalizeAngle(double angle); // angle in radians.

bool isAngleBetween(double angle, double minAngle, double maxAngle);


bool isPointInSectorAnnulus(Point p, Point center, double minAngle, double maxAngle, double minRange, double maxRange); 

bool getPolarBoxIndex(Point p, double c_x, double c_y,const Polar_box* box, int *range_idx, int *angle_idx);


void writeCartGridToFile(Cart_grid* cg, int scan_id, int what_to_print); 

Vol_scan *init_vol_scan(Cart_grid *cart_grids, int num_PPIs);

static inline int vol_index(const Vol_scan *vol, int x, int y, int ppi) {
    return ppi * vol->num_elements + x * vol->num_y + y;
}

int add_cart_grid_to_volscan(Vol_scan *vol, Cart_grid *grid, int ppi_index);

void free_vol_scan(Vol_scan *vol);


int write_vol_scan_ppi_to_file(const Vol_scan *vol, int ppi_index, const char *filename);


int compute_display_grid_max(Vol_scan *vol);
int write_display_grid_to_file(const Vol_scan *vol, const char *filename); 
#endif /* PROCESSING_H  */
