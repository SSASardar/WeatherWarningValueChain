


#ifndef PROCESSING_H
#define PROCESSING_H

typedef struct Cart_grid {
	double resolution;
	double *grid;
	int num_elements;
	int num_x;
	int num_y;
	Point ref_point;
} Cart_grid;


Cart_grid* Cart_grid_init(double resolution, int num_x, int num_y, Point ref_point);


#endif /* PROCESSING_H  */
