#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "logic.h"

#define MAX_INPUT 256

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

    // Remove trailing newline
    input[strcspn(input, "\n")] = 0;

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

int main() {
    int ref_string[MAX_PAGES];
    int num_pages, num_frames;

    printf("Page Replacement Algorithm Simulator\n");
    get_user_input(ref_string, &num_pages, &num_frames);

    // Run simulations and collect results
    SimulationResult fifo_result = run_simulation(ref_string, num_pages, num_frames, FIFO);
    SimulationResult lru_result = run_simulation(ref_string, num_pages, num_frames, LRU);
    SimulationResult opt_result = run_simulation(ref_string, num_pages, num_frames, OPTIMAL);

    // Summary of results
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
