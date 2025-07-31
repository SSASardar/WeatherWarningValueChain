#ifndef CONTROL_CENTRE_H
#define CONTROL_CENTRE_H
#include <stdbool.h>  // for bool type
#include <stdio.h>


// includes from external header files.

#include "common.h"
#include "radars.h"
#include "material_coords_raincell.h"
#include "spatial_coords_raincell.h"




// -----------------------------
// Command Structure Definition
// -----------------------------
typedef struct Command {
    int command_id;       // Unique command ID
    double time;          // Scheduled time
    int radar_id;         // Radar identifier
    char scan_mode[4];    // "RHI" or "PPI"
    int raincell_id;      // Raincell identifier
} Command;

// -----------------------------
// Public Function Declarations
// -----------------------------


/**
 * Defining a global log file
 */
 
extern FILE *log_file;


/**
 * Reads and processes all commands from the specified file.
 * Returns true on success, false on error.
 */
bool read_commands_from_file(const char *filename);

/**
 * Validates the given command against radar, scan mode, and raincell checks.
 * Returns true if valid, false if any check fails.
 */
bool validate_command(const Command *cmd);

/**
 * Executes the given command by dispatching it to internal tasks.
 */
void execute_command(const Command *cmd);

/**
 * Starts monitoring the inputs/ directory and processes command files in 5-minute intervals.
 * This function runs indefinitely unless externally stopped.
 */
void monitor_and_process_inputs(void);

void printCommand(const Command* cmd);

#endif // CONTROL_CENTRE_H
