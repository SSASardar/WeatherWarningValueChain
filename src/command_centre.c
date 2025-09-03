#include "control_centre.h"
#include "common.h"
#include "radars.h"
#include "material_coords_raincell.h"
#include "spatial_coords_raincell.h"
#include "vertical_profiles.h"
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <math.h>

static int global_command_id = 0;


// ---------------------- Logging ----------------------
FILE *log_file = NULL;

FILE *open_log_file_with_timestamp() {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    if (!t) return NULL;

    char filename[256];
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
    fflush(log_file);
    va_end(args);
}

// ---------------------- Command Generation ----------------------
#define SCANS_PER_FILE 16
// #define FIVE_MINUTES 300.0  // seconds
#define FIVE_MINUTES 5.0  // minutes

void generate_commands_file(int file_index, double start_time) {
    char filename[256];
    snprintf(filename, sizeof(filename), "inputs/commands_%04d.txt", file_index);

    FILE *file = fopen(filename, "w");
    if (!file) {
        fprintf(stderr, "Failed to create command file %s\n", filename);
        return;
    }

    double interval = FIVE_MINUTES / (SCANS_PER_FILE);  // 1/16th of 5 minutes
    int counter_A = 0;
    for (int i = 0; i < SCANS_PER_FILE; i++) {
        Command cmd;
        cmd.time = start_time + i * interval;
        cmd.radar_id = 1;
        snprintf(cmd.scan_mode, sizeof(cmd.scan_mode), "PPI");
        cmd.raincell_id = 1;
	cmd.other_angle = counter_A * ((M_PI_4/4) / (SCANS_PER_FILE+1)); // Exampe: 0–90
        counter_A++;
								 //
	if (counter_A == 8) counter_A =0;
        fprintf(file, "%.2f %d %s %d %.5f\n",
                cmd.time,
                cmd.radar_id,
                cmd.scan_mode,
                cmd.raincell_id,
                cmd.other_angle);
    }

    fclose(file);
    printf("Command file '%s' created successfully.\n", filename);
}

// ---------------------- Command Validation ----------------------
bool validate_command(const Command *cmd) {
    if (cmd->radar_id != 1 && cmd->radar_id != 2) {
        log_message("Validation Error: Radar ID %d does not exist. Command ID %d.\n",
                    cmd->radar_id, cmd->command_id);
        return false;
    }
    if (strcmp(cmd->scan_mode, "PPI") != 0 && strcmp(cmd->scan_mode, "RHI") != 0) {
        log_message("Validation Error: Invalid scan mode '%s'. Command ID %d.\n",
                    cmd->scan_mode, cmd->command_id);
        return false;
    }
    if (cmd->raincell_id != 1) {
        log_message("Validation Error: Raincell ID %d does not exist. Command ID %d.\n",
                    cmd->raincell_id, cmd->command_id);
        return false;
    }
    return true;
}

// ---------------------- Command Execution ----------------------
void printCommand(const Command* cmd) {
    if (!cmd) return;
    printf("Command ID: %d\n", cmd->command_id);
    printf("Scheduled Time: %.2f\n", cmd->time);
    printf("Radar ID: %d\n", cmd->radar_id);
    printf("Scan Mode: %.3s\n", cmd->scan_mode);
    printf("Raincell ID: %d\n", cmd->raincell_id);
    printf("Other Angle: %.2f\n", cmd->other_angle);
    printf("============ \n\n");
}
/*
void execute_command(const Command *cmd,Polar_box *box, const char *filename) {
    if (!cmd) {
        log_message("Fatal Error: NULL command pointer passed to execute_command().\n");
        return;
    }
    log_message("Executing command ID %d...\n", cmd->command_id);
   // printCommand(cmd);
   

    if (strcmp(cmd->scan_mode, "PPI") == 0) {
 
	    // Find radar associated with polar box
    const  Radar* found_radar = find_radar_by_id_ONLY(cmd->radar_id);
    if (found_radar) {
        printf("Radar found: ID = %d, Frequency = %s, Max Range = %.2f\n",
               get_radar_id(found_radar), get_frequency(found_radar), get_max_range_radar(found_radar));
    } else {
        printf("Radar not found.\n");
    }

   const Raincell* rc = find_raincell_by_id_ONLY(cmd->raincell_id);
if (rc != NULL) {
    printf("Found Raincell with ID %d, core radius = %f\n", rc->id, rc->radius_core);
} else {
    printf("Raincell not found!\n");
} 

const Spatial_raincell* s_rc = find_spatial_raincell_by_id_ONLY(cmd->raincell_id);
if (s_rc != NULL) {
    printf("Found Spatial_raincell with ID %d at (%.2f, %.2f) initially\n",
           s_rc->id, s_rc->initial_x, s_rc->initial_y);
} else {
    printf("Spatial_raincell not found!\n");
}



    update_other_angle(box, cmd->other_angle);

	// Fill polar box
	    if (fill_polar_box(box, cmd->time, s_rc, found_radar, rc) != 0) {
        printf("Failed to update polar box at time  %lf\n", cmd->time);
        // handle error if needed
    }


    Bounding_box* bounded_polar_box = create_bounding_box_for_polar_box_EZ(box);

print_bounding_box(bounded_polar_box);


fill_polar_box_grid(box, found_radar, s_rc, rc, cmd->time);

save_polar_box_grid_to_file(box, found_radar, cmd->local_scan_id, cmd->time,filename);

}
printf("===================\n\n");
}
*/

void execute_command(const Command *cmd, Polar_box *box, const char *filename, const VPR *vpr_strat ,const VPR_params *params, VPR *vpr_conv) {
    if (!cmd || !box) {
        log_message("Fatal: NULL pointer in execute_command\n");
        return;
    }

    log_message("Executing command ID %d...\n", cmd->command_id);

    const Radar* radar = find_radar_by_id_ONLY(cmd->radar_id);
    if (!radar) { log_message("Radar %d not found\n", cmd->radar_id); return; }

    const Raincell* rc = find_raincell_by_id_ONLY(cmd->raincell_id);
    if (!rc) { log_message("Raincell %d not found\n", cmd->raincell_id); return; }

    const Spatial_raincell* s_rc = find_spatial_raincell_by_id_ONLY(cmd->raincell_id);
    if (!s_rc) { log_message("Spatial Raincell %d not found\n", cmd->raincell_id); return; }

double time_in_min = cmd->time;
double time_in_sec = cmd->time * 60.0;
    // Fill and compute polar box
    if (fill_polar_box(box, time_in_sec, s_rc, radar, rc) != 0) {
        log_message("Failed to fill polar box for command ID %d\n", cmd->command_id);
        return;
    }


	update_other_angle(box, cmd->other_angle);


    Point* pos_raincell = get_position_raincell(time_in_sec, s_rc);
log_message("Radar=(%.1f, %.1f), Raincell=(%.1f, %.1f), time=%.1f, angle=%.3f rad\n",
            radar->x, radar->y,
            pos_raincell->x, pos_raincell->y,
            time_in_sec,
            box->other_angle);
free(pos_raincell);

update_VPR(vpr_strat, params, time_in_sec, vpr_conv);



    fill_polar_box_grid(box, radar, s_rc, rc, time_in_sec, vpr_strat, vpr_conv);
    save_polar_box_grid_to_file(box, radar, cmd->local_scan_id, time_in_min, filename);
}

bool read_command_file_once(const char *filename, const VPR *vpr_strat, const VPR_params *params, VPR *vpr_conv) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        log_message("Error opening file '%s': %s\n", filename, strerror(errno));
        return false;
    }

    // Allocate one Polar_box and reuse it
    Polar_box* box = init_polar_box();
    if (!box) {
        fclose(file);
        log_message("Failed to initialize polar box\n");
        return false;
    }

    Command cmd;
    while (fscanf(file, "%lf %d %3s %d %lf",
                  &cmd.time,
                  &cmd.radar_id,
                  cmd.scan_mode,
                  &cmd.raincell_id,
                  &cmd.other_angle) == 5) {

        cmd.local_scan_id = global_command_id % SCANS_PER_FILE;
        cmd.command_id = (int)floor(global_command_id / SCANS_PER_FILE);
        global_command_id++;

        log_message("Processing command ID %d: scan = %d, time=%.2f, radar_id=%d, scan_mode=%s, raincell_id=%d, angle=%.2f\n",
                    cmd.command_id, cmd.local_scan_id, cmd.time, cmd.radar_id, cmd.scan_mode, cmd.raincell_id, cmd.other_angle);

        if (!validate_command(&cmd)) {
            log_message("Command ID %d failed validation. Skipping.\n", cmd.command_id);
            continue;
        }

        // Safe output file name
        char filenameA[256];
        snprintf(filenameA, sizeof(filenameA), "outputs/radar_scan_%.4d.txt", cmd.command_id);

        // Execute the command safely
        execute_command(&cmd, box, filenameA, vpr_strat, params, vpr_conv);
    }

    // Free polar box once after all commands are done
    free_polar_box(box);
    fclose(file);

    log_message("Finished processing file: %s\n", filename);
    return true;
}

// ---------------------- Monitor Inputs ----------------------
void monitor_and_process_inputs(const VPR *vpr_strat, const VPR_params *params,  VPR *vpr_conv) {
    int file_index = 0;

    if (mkdir("logs", 0755) == -1 && errno != EEXIST) {
        fprintf(stderr, "Failed to create logs directory: %s\n", strerror(errno));
        return;
    }
    if (mkdir("archive", 0755) == -1 && errno != EEXIST) {
        fprintf(stderr, "Failed to create archive directory: %s\n", strerror(errno));
        return;
    }

    log_file = open_log_file_with_timestamp();
    if (!log_file) {
        fprintf(stderr, "Failed to open log file\n");
        return;
    }

    log_message("Monitoring started...\n");

    while (1) {
        char filename[256];
        snprintf(filename, sizeof(filename), "inputs/commands_%04d.txt", file_index);

        char archive_filename[256];
        snprintf(archive_filename, sizeof(archive_filename), "archive/commands_%04d.txt", file_index);

        if (access(filename, F_OK) == 0) {
            log_message("Detected file: %s. Beginning processing...\n", filename);

            if (read_command_file_once(filename, vpr_strat, params, vpr_conv)) {
                log_message("Successfully processed file: %s\n", filename);
            } else {
                log_message("Failed to process file: %s\n", filename);
            }

            if (rename(filename, archive_filename) != 0) {
                log_message("Failed to archive file %s: %s\n", filename, strerror(errno));
            } else {
                log_message("Archived file %s to %s\n", filename, archive_filename);
            }

            file_index++;
        } else {
            log_message("No file found for: %s. Stopping monitoring.\n", filename);
            break;
        }
    }

    log_message("Monitoring ended.\n");
    fclose(log_file);
    log_file = NULL;
}

