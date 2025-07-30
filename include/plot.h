#ifndef PLOT_H
#define PLOT_H


#include <plplot/plplot.h>



// Initialize and finalize plot system
void setup_plot(const char* filename);
void finalize_plot(void);

// Your actual plotting function
void plot_example(void);

#endif // PLOT_H
