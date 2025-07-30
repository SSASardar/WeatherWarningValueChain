#include "plot.h"
#include <plplot/plplot.h>

void setup_plot(const char* filename) {
//    plsdev("pdfcairo");     // Set output device to PDF
    plsdev("psc");
    plsfnam(filename);      // Set output filename
    plinit();               // Initialize PLplot
}

void finalize_plot(void) {
    plend();                // Finalize PLplot
}

void plot_example(void) {
    plenv(0, 1, 0, 1, 0, 0);
    pllab("x-axis", "y-axis", "Example Plot");
    PLFLT x[2] = {0.0, 1.0};                             
    PLFLT y[2] = {0.0, 1.0};                             
    plline(2, x, y);  // draw line

}
