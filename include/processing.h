


#ifndef PROCESSING_H
#define PROCESSING_H

#include<stdbool.h>
#include"radars.h"
#include <stdio.h>

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


Cart_grid* Cart_grid_init(double resolution, int num_x, int num_y, Point ref_point);

double normalizeAngle(double angle); // angle in radians.

bool isAngleBetween(double angle, double minAngle, double maxAngle);


bool isPointInSectorAnnulus(Point p, Point center, double minAngle, double maxAngle, double minRange, double maxRange); 

bool getPolarBoxIndex(Point p, double c_x, double c_y,const Polar_box* box, int *range_idx, int *angle_idx);


void writeCartGridToFile(Cart_grid* cg, int scan_id); 


#endif /* PROCESSING_H  */
