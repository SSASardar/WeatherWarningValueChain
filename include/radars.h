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
#include "common.h"

// Opaque defition of the radar structure
typedef struct Radar Radar;
typedef struct Polar_box Polar_box;


Radar* create_radar(int id, const char* frequency, const char* scanning_mode, double x, double y,double z, double max_range, double range_res, double angular_res);

//Polar_box* create_polar_box(double time, const Spatial_raincell* s_raincell, const Radar* radar, const Raincell* raincell);


Polar_box* init_polar_box();

int fill_polar_box(Polar_box* polar_box, double time, const Spatial_raincell* s_raincell, const Radar* radar, const Raincell* raincell);

void print_radar_specs(const Radar* radar);

Point* get_position_radar(const Radar* radar);
double get_height_of_radar(const Radar* radar);
double get_range_res_radar(const Radar* radar);

double get_angular_res_radar(const Radar* radar);

double get_max_range_radar(const Radar* radar);

int get_radar_id(const Radar* radar);

const char* get_frequency(const Radar* r);

const char* get_scanning_mode(const Radar*);



void print_polar_grid(const Polar_box* polar_box, const Radar** radars, int num_radars);

// Getter function declarations
int get_radar_id_for_polar_box(const struct Polar_box* box);
double get_min_range_gate(const struct Polar_box* box);
double get_max_range_gate(const struct Polar_box* box);
double get_min_angle(const struct Polar_box* box);
double get_max_angle(const struct Polar_box* box);
double get_num_ranges(const struct Polar_box* box);
double get_num_angles(const struct Polar_box* box);
double get_range_res_polar_box(const struct Polar_box* box);
double get_angular_res_polar_box(const struct Polar_box* box);


const Radar* find_radar_by_id(const Polar_box* box, const Radar** radars, int num_radars);

Bounding_box* create_bounding_box_for_polar_box(const Polar_box* p_box, const Radar** radars, int num_radars);

double calculate_height_of_beam_at_range(double range, double elevation, double mean_earth_radius, double height_of_radar);


int sample_from_relative_location_in_raincell(double range, double angle, const Point* radar_centre, const Point* spatial_centre, const Raincell* raincell);

void fill_polar_box_grid(struct Polar_box* box, const struct Radar* radar, const struct Spatial_raincell* s_raincell, const struct Raincell* raincell, double time);

void save_polar_box_grid_to_file(const Polar_box* box, const Radar* radar, int scan_index, const char* filename);

#endif /* RADARS_H */
