// This is a .c file
//
//
//
//
//





// include statements: 
#include "material_coords_raincell.h"
#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>


//global registries:

Raincell* raincell_list[MAX_RAINCELLS];
int raincell_count = 0;



Raincell* create_raincell(int id, double relative_size_core, double radius_stratiform, double relative_offset, double top_height_stratiform,/*  double top_height_core,*/ double max_top_height_core, double onset_time_growth, double stop_time_growth, double onset_time_decay, double stop_time_decay) {

	Raincell* raincell = malloc(sizeof(Raincell));
	raincell->id =  id;
	raincell->radius_core = relative_size_core*radius_stratiform;
	raincell->radius_stratiform = radius_stratiform;
	raincell->offset_centre_core = relative_offset*radius_stratiform;
	raincell->top_height_stratiform = top_height_stratiform;
	// raincell->top_height_core = top_height_core;
	raincell-> max_top_height_core = max_top_height_core;
	raincell->onset_time_growth = onset_time_growth;
	raincell->stop_time_growth = stop_time_growth;
	raincell->onset_time_decay = onset_time_decay;
	raincell->stop_time_decay = stop_time_decay;
	
//	printf("\nI just crafted raincell %d\n", raincell->id);
	return raincell;

}



void print_raincell(const Raincell* raincell){

printf("\n\nRaincell %d: \n 	Centre: 	(0,0)\n 	Radius:		%.2lf\n ", raincell->id, raincell->radius_stratiform);
printf("\nIts core has: \n 	Centre: 	(%.2lf, %.2lf)\n 	Radius: 	%.2lf\n", raincell->offset_centre_core, 0.00, raincell->radius_core);
printf("\n\nThe phases of the raincell development happen at the following times (minutes)\n Growth phase:	from %.2lf to %.2lf\n Mature phase:	from %.2lf to %.2lf\n Decay phase: 	from %.2lf to %.2lf\n\n\n", raincell->onset_time_growth / 60, raincell->stop_time_growth / 60, raincell->stop_time_growth / 60,raincell->onset_time_decay / 60,raincell->onset_time_decay / 60,raincell->stop_time_decay / 60);

}



void free_raincell(Raincell* raincell){
	int id_d = raincell->id;
	free(raincell);
//	printf("I just freed raincell %d\n\n", id_d);
}

// Getters for Raincell fields
int raincell_get_id(const Raincell* raincell) {
    return raincell->id;
}

double raincell_get_radius_core(const Raincell* raincell) {
    return raincell->radius_core;
}

double raincell_get_radius_stratiform(const Raincell* raincell) {
    return raincell->radius_stratiform;
}

double raincell_get_offset_centre_core(const Raincell* raincell) {
    return raincell->offset_centre_core;
}

double raincell_get_top_height_stratiform(const Raincell* raincell) {
    return raincell->top_height_stratiform;
}

double raincell_get_top_height_core(const Raincell* raincell, double time) {
    if(time<=raincell->onset_time_growth){
		return raincell-> top_height_stratiform;
	//	return 1.0;	
	}
	else if (time<=raincell->stop_time_growth){
	 return (raincell->max_top_height_core - raincell->top_height_stratiform)* ((time-raincell->onset_time_growth)/(raincell->stop_time_growth - raincell->onset_time_growth)) + raincell->top_height_stratiform;
	//return 2.0;
	}
	else if (time<=raincell->onset_time_decay){
	return raincell->max_top_height_core;
	//return 3.0;
	}
	else if (time<=raincell->stop_time_decay){
//	 return raincell->max_top_height_core - ((raincell->stop_time_decay-time)/(raincell->onset_time_decay - raincell->stop_time_decay))*(raincell->top_height_stratiform - raincell->max_top_height_core);
//	return 4.0;
	
	return raincell->top_height_stratiform + (raincell->stop_time_decay - time)/(raincell->stop_time_decay - raincell->onset_time_decay)*(raincell->max_top_height_core - raincell->top_height_stratiform);

	}
	else if (time > raincell->stop_time_decay){
	return raincell->top_height_stratiform;
//	return 5.0;
	}
	else {
	return -1.0;
	}


	//return raincell->top_height_core;
}

/* double raincell_get_time(const Raincell* raincell) {
    return raincell->time;
}
*/


// Uses the global raincell_list and raincell_count
const Raincell* find_raincell_by_id_ONLY(int idA) {
    for (int i = 0; i <raincell_count; ++i) {
        if (raincell_list[i]->id == idA) {
            return raincell_list[i];  // Found
        }
    }
    printf("find_raincell_by_id_ONLY()\nRaincell with id %d was not found. Returning NULL\n\n", idA);
    return NULL; // Not found
}

