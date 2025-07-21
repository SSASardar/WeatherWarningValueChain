// This is an empy .h file for the true vertical profile.h file

#ifndef TRUE_VERTICAL_PROFILE_H
#define TRUE_VERTICAL_PROFILE_H

extern double height_base;
extern double height_max_echo_top;

double reflectivity(double height);
void initialise_profile_variables();
void draw_reflectivity_bar(double height, double reflectivity);




#endif // TRUE_VERTICAL_PROFILE_H




