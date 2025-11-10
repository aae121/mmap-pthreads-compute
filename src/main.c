#include "compute.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define DATA_PATH   "data/numbers.txt"
#define N_PROC      4
#define N_THREADS   4

// Function to calculate time difference
static double elapsed(const struct timespec *a, const struct timespec *b) {
    return (b->tv_sec - a->tv_sec) + (b->tv_nsec - a->tv_nsec) / 1e9;
}

// Helper to log each result into CSV file
void log_result_to_csv(const char *method, int n, double time, unsigned long result) {
    FILE *fp = fopen("performance_data.csv", "a");
    if (fp) {
        fprintf(fp, "%d,%s,%.6f,%lu\n", n, method, time, result);
        fclose(fp);
    }
}

int main(void) {
    struct timespec t1, t2;

    // Define test sizes for many data points
    int sizes[] = {1000, 5000, 10000, 20000, 50000, 100000, 200000, 500000, 1000000};
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);

    // Create / overwrite CSV header
    FILE *fp = fopen("performance_data.csv", "w");
    if (fp) {
        fprintf(fp, "input_size,method,time_seconds,result\n");
        fclose(fp);
    }

    // Loop over all test sizes
    for (int i = 0; i < num_sizes; i++) {
        int n = sizes[i];
        char cmd[256];
        printf("\n📊 Testing input size = %d\n", n);

        // Generate data file for this test
        sprintf(cmd, "mkdir -p data && seq 1 %d > %s", n, DATA_PATH);
        system(cmd);

        // Sequential
        clock_gettime(CLOCK_MONOTONIC, &t1);
        int seq = sequential_compute(DATA_PATH, add_func);
        clock_gettime(CLOCK_MONOTONIC, &t2);
        double seq_time = elapsed(&t1, &t2);
        printf("sequential: %d (%.6f s)\n", seq, seq_time);
        log_result_to_csv("sequential", n, seq_time, seq);

        // Pipes
        clock_gettime(CLOCK_MONOTONIC, &t1);
        int pipes = pipes_compute(N_PROC, DATA_PATH, add_func);
        clock_gettime(CLOCK_MONOTONIC, &t2);
        double pipes_time = elapsed(&t1, &t2);
        printf("pipes (%d): %d (%.6f s)\n", N_PROC, pipes, pipes_time);
        log_result_to_csv("pipes", n, pipes_time, pipes);

        // mmap
        clock_gettime(CLOCK_MONOTONIC, &t1);
        int mm = mmap_compute(N_PROC, DATA_PATH, add_func);
        clock_gettime(CLOCK_MONOTONIC, &t2);
        double mmap_time = elapsed(&t1, &t2);
        printf("mmap (%d): %d (%.6f s)\n", N_PROC, mm, mmap_time);
        log_result_to_csv("mmap", n, mmap_time, mm);

        // pthreads
        clock_gettime(CLOCK_MONOTONIC, &t1);
        int thr = threads_compute(N_THREADS, DATA_PATH, add_func);
        clock_gettime(CLOCK_MONOTONIC, &t2);
        double thr_time = elapsed(&t1, &t2);
        printf("threads (%d): %d (%.6f s)\n", N_THREADS, thr, thr_time);
        log_result_to_csv("threads", n, thr_time, thr);
    }

    printf("\n✅ Results saved to performance_data.csv\n");
    return 0;
}
