// This is the common.c file.
//
//
//


#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "common.h"


double get_bounding_box_time(const Bounding_box* box) {
    return box->time;
}

Point get_bounding_box_top_left(const Bounding_box* box) {
    return box->topLeft;
}

Point get_bounding_box_top_right(const Bounding_box* box) {
    return box->topRight;
}

Point get_bounding_box_bottom_left(const Bounding_box* box) {
    return box->bottomLeft;
}

Point get_bounding_box_bottom_right(const Bounding_box* box) {
    return box->bottomRight;
}

// Helper to check if a point is inside an axis-aligned bounding box
int is_point_inside_box(const Point* p, const Bounding_box* box) {
    // Compute bounds of box
    double min_x = fmin(fmin(box->topLeft.x, box->topRight.x),
                        fmin(box->bottomLeft.x, box->bottomRight.x));
    double max_x = fmax(fmax(box->topLeft.x, box->topRight.x),
                        fmax(box->bottomLeft.x, box->bottomRight.x));

    double min_y = fmin(fmin(box->topLeft.y, box->topRight.y),
                        fmin(box->bottomLeft.y, box->bottomRight.y));
    double max_y = fmax(fmax(box->topLeft.y, box->topRight.y),
                        fmax(box->bottomLeft.y, box->bottomRight.y));

    return (p->x >= min_x && p->x <= max_x &&
            p->y >= min_y && p->y <= max_y);
}

int  does_box1_include_box2(const Bounding_box* box1, const Bounding_box* box2) {
    return is_point_inside_box(&box2->topLeft, box1) &&
           is_point_inside_box(&box2->topRight, box1) &&
           is_point_inside_box(&box2->bottomLeft, box1) &&
           is_point_inside_box(&box2->bottomRight, box1);
}
void print_bounding_box(const Bounding_box* box) {
    printf("Bounding Box:\n");

    printf("  Top Left     : (%.2f, %.2f)\n", box->topLeft.x, box->topLeft.y);
    printf("  Top Right    : (%.2f, %.2f)\n", box->topRight.x, box->topRight.y);
    printf("  Bottom Left  : (%.2f, %.2f)\n", box->bottomLeft.x, box->bottomLeft.y);
    printf("  Bottom Right : (%.2f, %.2f)\n", box->bottomRight.x, box->bottomRight.y);

    printf("\nASCII Representation:\n");

    printf("  +------------+\n");
    printf("  |            |\n");
    printf("  |            |\n");
    printf("  |            |\n");
    printf("  +------------+\n");

    printf("\n");
}


