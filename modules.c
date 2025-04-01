#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>

#define MAX_PAGES 100
#define MAX_FRAMES 10

// Function to print the current memory state
void print_memory(int frames[], int num_frames) {
    printf("Memory: [");
    for (int i = 0; i < num_frames; i++) {
        if (frames[i] == -1) {
            printf("-");
        } else {
            printf("%d", frames[i]);
        }
        if (i < num_frames - 1) {
            printf(", ");
        }
    }
    printf("]\n");
}

// Function to check if a string is numeric
int is_numeric(const char *str) {
    while (*str) {
        if (!isdigit(*str)) return 0;
        str++;
    }
    return 1;
}

// Function to read the page reference string from user input
void read_reference_string(int ref_string[], int *num_pages) {
    char input[256];
    printf("Enter the number of pages: ");
    scanf("%d", num_pages);
    if (*num_pages > MAX_PAGES || *num_pages <= 0) {
        printf("Error: Number of pages must be between 1 and %d.\n", MAX_PAGES);
        exit(1);
    }
    printf("Enter the page reference string (space-separated): ");
    getchar(); // Consume the newline
    fgets(input, sizeof(input), stdin);
    
    // Remove trailing newline
    size_t len = strlen(input);
    if (len > 0 && input[len - 1] == '\n') {
        input[len - 1] = '\0';
    }
    
    char *token = strtok(input, " ");
    int i = 0;
    while (token != NULL && i < *num_pages) {
        if (!is_numeric(token)) {
            printf("Error: '%s' is not a valid page number.\n", token);
            exit(1);
        }
        ref_string[i++] = atoi(token);
        token = strtok(NULL, " ");
    }
    if (i < *num_pages) {
        printf("Error: Expected %d pages, but only %d were provided.\n", *num_pages, i);
        exit(1);
    }
}

// Function to find if a page exists in memory and return its index
int find_page(int frames[], int num_current, int page) {
    for (int i = 0; i < num_current; i++) {
        if (frames[i] == page) {
            return i;
        }
    }
    return -1;
}

// FIFO Page Replacement Algorithm
void fifo(int ref_string[], int num_pages, int num_frames) {
    int frames[MAX_FRAMES];
    int num_current = 0;
    int faults = 0;
    
    for (int i = 0; i < num_frames; i++) {
        frames[i] = -1;
    }
    
    printf("\nAlgorithm: FIFO\n");
    for (int i = 0; i < num_pages; i++) {
        int page = ref_string[i];
        printf("Page Request: %2d | ", page);
        
        int found = find_page(frames, num_current, page);
        if (found != -1) {
            printf("Page Fault: No  | ");
        } else {
            if (num_current < num_frames) {
                frames[num_current++] = page;
            } else {
                for (int j = 0; j < num_frames - 1; j++) {
                    frames[j] = frames[j + 1];
                }
                frames[num_frames - 1] = page;
            }
            faults++;
            printf("Page Fault: Yes | ");
        }
        print_memory(frames, num_frames);
    }
    printf("Total Page Faults: %d\n", faults);
}

// LRU Page Replacement Algorithm
void lru(int ref_string[], int num_pages, int num_frames) {
    int frames[MAX_FRAMES];
    int num_current = 0;
    int faults = 0;
    
    for (int i = 0; i < num_frames; i++) {
        frames[i] = -1;
    }
    
    printf("\nAlgorithm: LRU\n");
    for (int i = 0; i < num_pages; i++) {
        int page = ref_string[i];
        printf("Page Request: %2d | ", page);
        
        int index = find_page(frames, num_current, page);
        if (index != -1) {
            int temp = frames[index];
            for (int j = index; j < num_current - 1; j++) {
                frames[j] = frames[j + 1];
            }
            frames[num_current - 1] = temp;
            printf("Page Fault: No  | ");
        } else {
            if (num_current < num_frames) {
                frames[num_current++] = page;
            } else {
                for (int j = 0; j < num_frames - 1; j++) {
                    frames[j] = frames[j + 1];
                }
                frames[num_frames - 1] = page;
            }
            faults++;
            printf("Page Fault: Yes | ");
        }
        print_memory(frames, num_frames);
    }
    printf("Total Page Faults: %d\n", faults);
}

// Function to find the victim page for Optimal algorithm
int find_victim(int frames[], int num_current, int ref_string[], int num_pages, int current_i) {
    int max_distance = -1;
    int victim_index = -1;
    
    for (int j = 0; j < num_current; j++) {
        int page = frames[j];
        int distance = 0;
        for (int k = current_i + 1; k < num_pages; k++) {
            if (ref_string[k] == page) {
                distance = k - current_i;
                break;
            }
        }
        if (distance == 0) {
            return j;
        }
        if (distance > max_distance) {
            max_distance = distance;
            victim_index = j;
        }
    }
    return victim_index != -1 ? victim_index : 0;
}

// Optimal Page Replacement Algorithm
void optimal(int ref_string[], int num_pages, int num_frames) {
    int frames[MAX_FRAMES];
    int num_current = 0;
    int faults = 0;
    
    for (int i = 0; i < num_frames; i++) {
        frames[i] = -1;
    }
    
    printf("\nAlgorithm: Optimal\n");
    for (int i = 0; i < num_pages; i++) {
        int page = ref_string[i];
        printf("Page Request: %2d | ", page);
        
        int found = find_page(frames, num_current, page);
        if (found != -1) {
            printf("Page Fault: No  | ");
        } else {
            if (num_current < num_frames) {
                frames[num_current++] = page;
            } else {
                int victim = find_victim(frames, num_current, ref_string, num_pages, i);
                frames[victim] = page;
            }
            faults++;
            printf("Page Fault: Yes | ");
        }
        print_memory(frames, num_frames);
    }
    printf("Total Page Faults: %d\n", faults);
}

// Main function
int main() {
    int ref_string[MAX_PAGES];
    int num_pages;
    int num_frames;
    
    read_reference_string(ref_string, &num_pages);
    printf("Enter the number of frames: ");
    scanf("%d", &num_frames);
    if (num_frames > MAX_FRAMES || num_frames < 1) {
        printf("Error: Number of frames must be between 1 and %d.\n", MAX_FRAMES);
        return 1;
    }
    
    fifo(ref_string, num_pages, num_frames);
    lru(ref_string, num_pages, num_frames);
    optimal(ref_string, num_pages, num_frames);
    
    return 0;
}
