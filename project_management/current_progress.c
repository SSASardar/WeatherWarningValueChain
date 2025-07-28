#include <stdio.h>
#include <time.h>

#define MAX_TASKS 10
#define MAX_CATEGORIES 5

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
                {"Conceptual model",60},
		{"2D geometries (material)", 100},
		{"Vertical information (material)",40},
		{"Lowest Z",30},
                {"Cell motion (spatial)", 80},
            },
            5
        },
        {
            "Model of radar",
            {
                {"Radar geometry (PPI)", 40},
		{"Extra information RHI", 20},
		{"Noise model", 15},
		{"Attenuation model",10},
		{"Clutter model",10},
		{"Getting lowest Z",30},
		{"Measurement outputs",30},
            },
            7
        },
        {
            "Meteorological product",
            {
                {"Spatial interpolation", 30},
                {"Temporal interpolation", 30},
		{"Total rainfall accumulation",30},
		{"(opt) Forecast", 10}
            },
            4
        },
        {
            "Hydrological product",
            {
                {"Spatial interpolation", 15},
		{"Temporal interpolation", 10},
		{"Entry rate of water into river",10}
	
            },
            3
        },
        {
            "Comparative measures",
            {
                {"True rainfall accumulation", 25},
                {"True entry rate", 0}
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
        "\nLegend:\n"
        "  ****................ (≤ 30%%): Idea\n"
        "  **********.......... (≤ 50%%): Functioning implementation\n"
        "  ****************.... (≤ 85%%): Test-ready, but can be improved\n"
        "  ******************** (100%%): Complete and satisfying\n";

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
        "Implement the radar measurement model",
        "Figure out a noise model",
        "Figure out attenuation model"
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
