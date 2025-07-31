#include "control_centre.h"

#include <unistd.h>  // for sleep()
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

static int global_command_id = 0;

// External validation functions

#include "common.h"
#include "radars.h"
#include "material_coords_raincell.h"
#include "spatial_coords_raincell.h"


// Task dispatcher
FILE *log_file = NULL;  // Define the global log_file variable
FILE *open_log_file_with_timestamp() {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    if (!t) return NULL;

    char filename[256];
    // Format: control_centre_YYYY-MM-DD_HH-MM-SS.log
    strftime(filename, sizeof(filename), "logs/control_centre_%Y-%m-%d_%H-%M-%S.log", t);

    FILE *log_file = fopen(filename, "a");
    if (!log_file) {
        fprintf(stderr, "Failed to open log file %s\n", filename);
    }
    return log_file;
}

static void log_message(const char *format, ...) {
    if (!log_file) return;

    va_list args;
    va_start(args, format);
    vfprintf(log_file, format, args);
    fflush(log_file); // Ensure it's written immediately
    va_end(args);
}

bool read_commands_from_file(const char *filename) {
   // already opened in monitor files function...  log_file = fopen("logs/control_centre.log", "a");  // Append mode
    if (!log_file) {
        fprintf(stderr, "Failed to open log file: %s\n", strerror(errno));
        return false;
    }

    log_message("Reading command file: %s\n", filename);

    FILE *file = fopen(filename, "r");
    if (!file) {
        log_message("Error opening file '%s': %s\n", filename, strerror(errno));
        //fclose(log_file);
        return false;
    }

    Command cmd;
    while (fscanf(file, "%lf %d %3s %d", &cmd.time, &cmd.radar_id, cmd.scan_mode, &cmd.raincell_id) == 4) {
        cmd.command_id = global_command_id++;

        log_message("Processing command ID %d: time=%.2f, radar_id=%d, scan_mode=%s, raincell_id=%d\n",
                    cmd.command_id, cmd.time, cmd.radar_id, cmd.scan_mode, cmd.raincell_id);

        if (!validate_command(&cmd)) {
            log_message("Command ID %d failed validation. Skipping.\n", cmd.command_id);
            continue;
        }

        execute_command(&cmd);
    }

    fclose(file);
    log_message("Finished processing file: %s\n\n", filename);
   // as you are monitoring already...  fclose(log_file);
   // as you are monitoring already...  log_file = NULL;
    return true;
}

/*bool read_commands_from_file(const char *filename) {
    printf("Reading command file: %s\n", filename);

    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error opening file '%s': %s\n", filename, strerror(errno));
        return false;
    }

    Command cmd;
    while (fscanf(file, "%lf %d %3s %d", &cmd.time, &cmd.radar_id, cmd.scan_mode, &cmd.raincell_id) == 4) {
        cmd.command_id = global_command_id++;

        printf("Processing command ID %d: time=%.2f, radar_id=%d, scan_mode=%s, raincell_id=%d\n",
               cmd.command_id, cmd.time, cmd.radar_id, cmd.scan_mode, cmd.raincell_id);

        if (!validate_command(&cmd)) {
            fprintf(stderr, "Command ID %d failed validation. Skipping.\n", cmd.command_id);
            continue;
        }

        execute_command(&cmd);
    }

    fclose(file);
    return true;
}
*/


bool validate_command(const Command *cmd) {
    if (cmd->radar_id != 1 && cmd->radar_id != 2) {
        log_message("Validation Error: Radar ID %d does not exist. Command ID %d.\n", cmd->radar_id, cmd->command_id);
        return false;
    }
    if (strcmp(cmd->scan_mode,"PPI")!=0 && strcmp(cmd->scan_mode,"RHI")!=0) {
        log_message("Validation Error: Invalid scan mode '%s'. Command ID %d.\n", cmd->scan_mode, cmd->command_id);
        return false;
    }
    if (cmd->raincell_id != 1) {
        log_message("Validation Error: Raincell ID %d does not exist. Command ID %d.\n", cmd->raincell_id, cmd->command_id);
        return false;
    }
    return true;

	//log_message("Validation is not implemented yet\n");
	//return 1;
}

void execute_command(const Command *cmd) {
    if (cmd == NULL) {
        log_message("Fatal Error: NULL command pointer passed to execute_command().\n");
        return;
    }

    log_message("Executing command ID %d...\n", cmd->command_id);
    // do the tasks
	printCommand(cmd);
}

void monitor_and_process_inputs() {
    int minute_counter = 0;

    // Create logs directory if it doesn't exist
    if (mkdir("logs", 0755) == -1 && errno != EEXIST) {
        fprintf(stderr, "Failed to create logs directory: %s\n", strerror(errno));
        return;
    }

    // Create logs directory if it doesn't exist
	if (mkdir("archive", 0755) == -1 && errno != EEXIST) {
    	fprintf(stderr, "Failed to create archives directory: %s\n", strerror(errno));
    	return;
	}


    //log_file = fopen("logs/control_centre.log", "a");
    log_file = open_log_file_with_timestamp();
	if (!log_file) {
        fprintf(stderr, "Failed to open log file: %s\n", strerror(errno));
        return;
    }

    log_message("Monitoring started...\n");

    while (1) {
        char filename[256];
        snprintf(filename, sizeof(filename), "inputs/commands_%04d.txt", minute_counter);

	    	char archive_filename[256];
		snprintf(archive_filename, sizeof(archive_filename), "archive/commands_%04d.txt", minute_counter);


        if (access(filename, F_OK) == 0) {
            log_message("Detected file: %s. Beginning processing...\n", filename);

            if (read_commands_from_file(filename)) {
                log_message("Successfully processed file: %s\n", filename);
            } else {
                log_message("Failed to process file: %s\n", filename);
            }

		if (rename(filename, archive_filename) != 0) {
	    		log_message("Failed to archive file %s: %s\n", filename, strerror(errno));
		} else {
    			log_message("Archived file %s to %s\n", filename, archive_filename);
		}
	    minute_counter += 5;

        } else {
            // No file found for the current interval:
            log_message("No file found for: %s. Stopping monitoring.\n", filename);
            break;  // Exit the loop and stop monitoring
        }
    }

    log_message("Monitoring ended.\n");

    fclose(log_file);
    log_file = NULL;
}

void printCommand(const Command* cmd) {
    if (cmd == NULL) {
        printf("Command is NULL\n");
        return;
    }
    printf("Command ID: %d\n", cmd->command_id);
    printf("Scheduled Time: %.2f\n", cmd->time);
    printf("Radar ID: %d\n", cmd->radar_id);
    printf("Scan Mode: %.3s\n", cmd->scan_mode);  // Limit to 3 chars to avoid printing extra chars
    printf("Raincell ID: %d\n", cmd->raincell_id);
    printf("============ \n\n");
}
