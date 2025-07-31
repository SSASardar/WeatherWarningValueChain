#include <stdio.h>
#include <time.h>

#define MAX_TASKS 10
#define MAX_CATEGORIES 7

typedef struct {
    const char* task;
    int completion; // percentage
} Task;

typedef struct {
    const char* category;
    Task tasks[MAX_TASKS];
    int task_count;
} Category;

void print_progress() {
    Category categories[MAX_CATEGORIES] = {
        {
            "Model of rain",
            {
                {"Holistic overview",60},
		{"Material and spatial descriptions", 80},
		{"Development convective core",20},
		{"Dynamic equil. VPR", 15},
		{"What is the ground truth?",20}
            },
            5
        },
        {
            "Model of radar",
            {
                {"Radar geometry (PPI)", 50},
		{"Radar geometry (RHI)", 10},
		{"Noise model", 5},
		{"Attenuation model",5},
		{"Clutter model",5},
		{"Getting lowest Z",5},
		{"Measurement outputs",15}
            },
            7
        },
	{
		"Radar config. & contr.",
		{
			{"Reading in commands",80},
			{"Validating commands",70},
			{"Checking for new files",60},
			{"Saving radar outputs",0},
			{"Generating command list",0}
		},
		5
	},
	{
		"Processing unit",
		{
			{"Cartesian spatial interpolation", 15},
			{"Temporal interpolation", 5},
			{"PPI to rainfall rate", 20},
			{"PPI-PPI to rainfall rate", 10},
			{"PPI-PPI-RHI to rainfall rate", 5},
			{"PPI-RHI to rainfall rate", 5},

		},
		6
	},
        {
            "Meteorological product",
            {
		{"Total rainfall accumulation",30},
		{"True rainfall accumulation",5},
		{"(opt) Forecast", 0}
            },
            3
        },
        {
            "Hydrological product",
            {
		{"Entry rate of water into river",10},
		{"True rainfall accumulation",5},
		{"(opt) Forecast",0}
            },
            4
        },
        {
            "Comparative measures",
            {
                {"Rainfall acc. map + MSE/Bias", 15},
                {"Entry rate curve + MSE/Bias", 0}
            },
            2
        }
    };

    FILE* file = fopen("progress.txt", "w");
    if (!file) {
        perror("Error opening progress.txt");
        return;
    }

    const char* header =
        "+------------------------+----------------------------------+----------------------+\n"
        "|        Category        |                Task              |        Progress      |\n"
        "+------------------------+----------------------------------+----------------------+\n";

    printf("\n\n\n%s", header);
    fprintf(file, "%s", header);

    for (int i = 0; i < MAX_CATEGORIES; i++) {
        for (int j = 0; j < categories[i].task_count; j++) {
            char progress[21];
            int stars = categories[i].tasks[j].completion / 5;
            for (int k = 0; k < 20; k++) {
                progress[k] = (k < stars) ? '*' : '.';
            }
            progress[20] = '\0';

            const char* category_name = (j == 0) ? categories[i].category : "";

            printf("| %-22s | %-32s | %-20s |\n",
                   category_name,
                   categories[i].tasks[j].task,
                   progress);

            fprintf(file, "| %-22s | %-32s | %-20s |\n",
                    category_name,
                    categories[i].tasks[j].task,
                    progress);
        }
    }

    const char* footer =
        "+------------------------+----------------------------------+----------------------+\n";
    printf("%s", footer);
    fprintf(file, "%s", footer);
	
    const char* legend =
        "\n                                                         Legend:\n\n"
        "                                                              ****................ (≤ 30%%): Idea\n"
        "                                                              **********.......... (≤ 50%%): Functioning implementation\n"
        "                                                              ****************.... (≤ 85%%): Test-ready, but can be improved\n"
        "                                                              ******************** (100%%): Complete and satisfying\n";

    printf("\n\n\n%s\n\n\n", legend);
    fprintf(file, "%s", legend);

    // -----------------------------
    // Print current date and time
    // -----------------------------
    time_t now = time(NULL);
    char time_str[64];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", localtime(&now));


	    // -----------------------------
    // Manual: Next 3 tasks to do
    // -----------------------------
    const char* next_tasks[3] = {
        "Execute command function in control_centre.c file, making and setting a polar grid",
        "output radar files to a directory",
        "Derive equation for time dependent VPR?"
    };

    fprintf(file, "\n\n\nNext 3 tasks to prioritise (as of %s):\n", time_str);
    fprintf(file, "  1. %s\n", next_tasks[0]);
    fprintf(file, "  2. %s\n", next_tasks[1]);
    fprintf(file, "  3. %s\n", next_tasks[2]);

    printf("\nNext 3 tasks to prioritise (as of %s):\n", time_str);
    printf("  1. %s\n", next_tasks[0]);
    printf("  2. %s\n", next_tasks[1]);
    printf("  3. %s\n\n\n", next_tasks[2]);

    fclose(file);
}


int main(void){
	print_progress();
}
