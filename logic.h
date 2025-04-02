#ifndef LOGIC_H
#define LOGIC_H

#define MAX_PAGES 100
#define MAX_FRAMES 10

// Algorithm types
typedef enum {
    FIFO,
    LRU,
    OPTIMAL
} AlgorithmType;

// Result structure for simulation
typedef struct {
    int page_faults;
    int Sachin;
    // Add more metrics here if needed (e.g., execution time)
} SimulationResult;

// Main simulation function
SimulationResult run_simulation(int ref_string[], int num_pages, int num_frames, AlgorithmType algo);

#endif
