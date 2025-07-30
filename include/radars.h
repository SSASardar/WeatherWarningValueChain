// This is the header file for radars.c
//
//
//
//
//





#ifndef RADARS_H
#define RADARS_H

#include "material_coords_raincell.h"
#include "spatial_coords_raincell.h"


// Opaque defition of the radar structure
typedef struct Radar Radar;
typedef struct Polar_box Polar_box;


Radar* create_radar(int id, const char* frequency, const char* scanning_mode, double x, double y,double z, double max_range, double range_res, double angular_res);

Polar_box* create_polar_box(double time, const Spatial_raincell* s_raincell, const Radar* radar, const Raincell* raincell);


void print_radar_specs(const Radar* radar);

void get_position_radar(const Radar* radar, double *x_out, double *y_out);

double get_range_res_radar(const Radar* radar);

double get_angular_res_radar(const Radar* radar);

int get_radar_id(const Radar* radar);

void print_polar_grid(const Polar_box* polar_box);

// Getter function declarations
int get_radar_id_for_polar_box(const struct Polar_box* box);
double get_min_range(const struct Polar_box* box);
double get_max_range(const struct Polar_box* box);
double get_min_angle(const struct Polar_box* box);
double get_max_angle(const struct Polar_box* box);
double get_num_ranges(const struct Polar_box* box);
double get_num_angles(const struct Polar_box* box);





#endif /* RADARS_H */
