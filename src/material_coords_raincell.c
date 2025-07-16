// This is a .c file
//
//
//
//
//





// include statements: 
#include "material_coords_raincell.h"
#include <stdio.h>
#include <math.h>


// private function 

static double size_stratiform = 15000;
static double size_convective = 0.3*size_stratiform;
static double a = -0.5*size_stratiform;
static double c = 50000;



static double height_base = 1000;
static double height_max_echo_top = 8000;


static double height_brightband = 3500;
static double width_brightband = 1000;

static double height_brightband_l = height_brightband-width_brightband;
static double height_brightband_u = height_brightband+width_brightband;


static double reflectivity_base = 30;
static double reflectivity_max_echo_top = 20;


static double reflectivity_brightband = 60;
static double reflectivity_absdiff_brightband = 15;

static double reflectivity_brightband_l = height_brightband-reflectivity_absdiff_brightband*2/3;
static double reflectivity_brightband_u = height_brightband-reflectivity_absdiff_brightband/3;


 

// implementing a public function declared in material_coords_raincell.h file

void internal_geometry_raincell(void){
	printf("a cirlce inside another circle. one circle is the stratiform, the other is the convective core\n");
}


