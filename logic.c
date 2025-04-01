#include <stdio.h>
#include <string.h>
#include "logic.h"

static void print_memory(int frames[], int num_frames) {
    printf("Memory: [");
    for (int i = 0; i < num_frames; i++) {
        if (frames[i] == -1) printf(" -");
        else printf("%2d", frames[i]);
        if (i < num_frames - 1) printf(",");
    }
    printf(" ]\n");
}

static int find_page(int frames[], int num_current, int page) {
    for (int i = 0; i < num_current; i++) {
        if (frames[i] == page) return i;
    }
    return -1;
}

static void simulate_fifo(int ref_string[], int num_pages, int num_frames, int frames[], int *faults, int *num_current) {
    for (int i = 0; i < num_pages; i++) {
        int page = ref_string[i];
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

SimulationResult run_simulation(int ref_string[], int num_pages, int num_frames, AlgorithmType algo) {
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
