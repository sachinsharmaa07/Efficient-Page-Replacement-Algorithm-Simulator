#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PAGES 100
#define MAX_FRAMES 10
#define MAX_INPUT 256

// Algorithm types
typedef enum {
    FIFO,
    LRU,
    OPTIMAL
} AlgorithmType;

// Result structure for simulation
typedef struct {
    int page_faults;
} SimulationResult;

// Print current memory state
static void print_memory(int frames[], int num_frames) {
    printf("Memory: [");
    for (int i = 0; i < num_frames; i++) {
        if (frames[i] == -1) printf(" -");
        else printf("%2d", frames[i]);
        if (i < num_frames - 1) printf(",");
    }
    printf(" ]\n");
}

// Check if page exists in memory
static int find_page(int frames[], int num_current, int page) {
    for (int i = 0; i < num_current; i++) {
        if (frames[i] == page) return i;
    }
    return -1;
}

// FIFO simulation
static void simulate_fifo(int ref_string[], int num_pages, int num_frames, int frames[], int *faults, int *num_current) {
    for (int i = 0; i < num_pages; i++) {
        int page = ref_string[i];
        int anchl;
        printf("Page %2d | ", page);
        int found = find_page(frames, *num_current, page);
        if (found == -1) {
            if (*num_current < num_frames) {
                frames[(*num_current)++] = page;
            } else {
                memmove(frames, frames + 1, (*num_current - 1) * sizeof(int));
                frames[num_frames - 1] = page;
            }
            (*faults)++;
            printf("Fault | ");
        } else {
            printf("Hit   | ");
        }
        print_memory(frames, num_frames);
    }
}

// LRU simulation
static void simulate_lru(int ref_string[], int num_pages, int num_frames, int frames[], int *faults, int *num_current) {
    int use_time[MAX_FRAMES] = {0};
    for (int i = 0; i < num_pages; i++) {
        int page = ref_string[i];
        printf("Page %2d | ", page);
        int found = find_page(frames, *num_current, page);
        if (found != -1) {
            use_time[found] = i + 1;
            printf("Hit   | ");
        } else {
            int victim = 0;
            if (*num_current < num_frames) {
                victim = (*num_current)++;
            } else {
                int min_time = use_time[0];
                for (int j = 1; j < num_frames; j++) {
                    if (use_time[j] < min_time) {
                        min_time = use_time[j];
                        victim = j;
                    }
                }
            }
            frames[victim] = page;
            use_time[victim] = i + 1;
            (*faults)++;
            printf("Fault | ");
        }
        print_memory(frames, num_frames);
    }
}

// Find victim page for Optimal algorithm
static int find_optimal_victim(int frames[], int num_current, int ref_string[], int num_pages, int current_idx) {
    int farthest = -1, victim = 0;
    for (int i = 0; i < num_current; i++) {
        int next_use = num_pages;
        for (int j = current_idx + 1; j < num_pages; j++) {
            if (frames[i] == ref_string[j]) {
                next_use = j;
                break;
            }
        }
        if (next_use > farthest) {
            farthest = next_use;
            victim = i;
        }
    }
    return victim;
}

// Optimal simulation
static void simulate_optimal(int ref_string[], int num_pages, int num_frames, int frames[], int *faults, int *num_current) {
    for (int i = 0; i < num_pages; i++) {
        int page = ref_string[i];
        printf("Page %2d | ", page);
        int found = find_page(frames, *num_current, page);
        if (found == -1) {
            if (*num_current < num_frames) {
                frames[(*num_current)++] = page;
            } else {
                int victim = find_optimal_victim(frames, *num_current, ref_string, num_pages, i);
                frames[victim] = page;
            }
            (*faults)++;
            printf("Fault | ");
        } else {
            printf("Hit   | ");
        }
        print_memory(frames, num_frames);
    }
}

// Run simulation for a given algorithm
static SimulationResult run_simulation(int ref_string[], int num_pages, int num_frames, AlgorithmType algo) {
    int frames[MAX_FRAMES];
    int faults = 0, num_current = 0;
    memset(frames, -1, sizeof(frames));

    printf("\nRunning %s:\n", algo == FIFO ? "FIFO" : algo == LRU ? "LRU" : "Optimal");
    switch (algo) {
        case FIFO:    simulate_fifo(ref_string, num_pages, num_frames, frames, &faults, &num_current); break;
        case LRU:     simulate_lru(ref_string, num_pages, num_frames, frames, &faults, &num_current); break;
        case OPTIMAL: simulate_optimal(ref_string, num_pages, num_frames, frames, &faults, &num_current); break;
    }
    printf("Total Page Faults: %d\n", faults);

    SimulationResult result = {faults};
    return result;
}

// Read and validate user input
static void get_user_input(int ref_string[], int *num_pages, int *num_frames) {
    char input[MAX_INPUT];
    
    printf("Enter the number of pages (1-%d): ", MAX_PAGES);
    scanf("%d", num_pages);
    if (*num_pages <= 0 || *num_pages > MAX_PAGES) {
        fprintf(stderr, "Error: Number of pages must be between 1 and %d\n", MAX_PAGES);
        exit(EXIT_FAILURE);
    }

    printf("Enter the number of frames (1-%d): ", MAX_FRAMES);
    scanf("%d", num_frames);
    if (*num_frames <= 0 || *num_frames > MAX_FRAMES) {
        fprintf(stderr, "Error: Number of frames must be between 1 and %d\n", MAX_FRAMES);
        exit(EXIT_FAILURE);
    }

    printf("Enter the page reference string (space-separated, e.g., 1 2 3): ");
    getchar(); // Consume newline
    if (!fgets(input, MAX_INPUT, stdin)) {
        fprintf(stderr, "Error: Failed to read input\n");
        exit(EXIT_FAILURE);
    }

    input[strcspn(input, "\n")] = 0; // Remove trailing newline

    char *token = strtok(input, " ");
    int i = 0;
    while (token && i < *num_pages) {
        for (char *p = token; *p; p++) {
            if (!isdigit(*p)) {
                fprintf(stderr, "Error: '%s' is not a valid page number\n", token);
                exit(EXIT_FAILURE);
            }
        }
        ref_string[i++] = atoi(token);
        token = strtok(NULL, " ");
    }
    if (i != *num_pages) {
        fprintf(stderr, "Error: Expected %d pages, got %d\n", *num_pages, i);
        exit(EXIT_FAILURE);
    }
}

// Main function
int main() {
    int ref_string[MAX_PAGES];
    int num_pages, num_frames;

    printf("Page Replacement Algorithm Simulator\n");
    get_user_input(ref_string, &num_pages, &num_frames);

    SimulationResult fifo_result = run_simulation(ref_string, num_pages, num_frames, FIFO);
    SimulationResult lru_result = run_simulation(ref_string, num_pages, num_frames, LRU);
    SimulationResult opt_result = run_simulation(ref_string, num_pages, num_frames, OPTIMAL);

    printf("\nSimulation Summary:\n");
    printf("Algorithm | Page Faults | Hit Ratio\n");
    printf("----------|-------------|----------\n");
    printf("FIFO      | %11d | %.2f%%\n", fifo_result.page_faults, 
           (1.0 - (float)fifo_result.page_faults / num_pages) * 100);
    printf("LRU       | %11d | %.2f%%\n", lru_result.page_faults, 
           (1.0 - (float)lru_result.page_faults / num_pages) * 100);
    printf("Optimal   | %11d | %.2f%%\n", opt_result.page_faults, 
           (1.0 - (float)opt_result.page_faults / num_pages) * 100);

    return 0;
}
