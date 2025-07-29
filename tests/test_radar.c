#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "radars.h"
#include "material_coords_raincell.h"
#include "spatial_coords_raincell.h"


int main() {
    Radar* radar = create_radar(1, "X", "PPI", -50000.0, 50000.0, 25.0, 50000.0, 50.0, 1.0);
    printf("\n\n\n");
    print_radar_specs(radar);
    printf("\n\n\n");













    free(radar);  // Always free what you malloc
    return 0;
}
