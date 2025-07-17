// this is the main.c file
//
//
//
//







#include <stdio.h>
#include "material_coords_raincell.h"

int main(void){
	printf("Programme started.\n");


	Raincell* raincell = create_raincell(1, 0.3, 15000.0, -0.5, 1000.0, 1500.0, 0.0);
	print_raincell(raincell);
	free_raincell(raincell);


//internal_geometry_raincell();

printf("Programme finished.\n");
return 0;
}


