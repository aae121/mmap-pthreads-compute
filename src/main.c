#include "compute.h"
#include "utils.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define DATA_PATH   "data/numbers.txt"
#define N_PROC      16
#define N_THREADS   16

// Helper function to measure elapsed time
static double elapsed(const struct timespec *a, const struct timespec *b) {
    return (b->tv_sec - a->tv_sec) + (b->tv_nsec - a->tv_nsec) / 1e9;
}

int main(void) {
    FILE *fp = fopen("results.csv", "w");
    if (!fp) {
        perror("Error creating results.csv");
        return 1;
    }

    fprintf(fp, "N,Sequential,Pipes,Mmap,Threads\n");

    long N = 10000;  // Start small
    while (N <= 5000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000) {  // Go up to 50 million
        char cmd[128];
        snprintf(cmd, sizeof(cmd), "head -n %ld %s > temp.txt", N, DATA_PATH);
        system(cmd);

        struct timespec t1, t2;
        double seq_t, pipes_t, mmap_t, threads_t;
        int seq, pipes, mm, thr;

        // Sequential
        clock_gettime(CLOCK_MONOTONIC, &t1);
        seq = sequential_compute("temp.txt", add_func);
        clock_gettime(CLOCK_MONOTONIC, &t2);
        seq_t = elapsed(&t1, &t2);

        // Pipes
        clock_gettime(CLOCK_MONOTONIC, &t1);
        pipes = pipes_compute(N_PROC, "temp.txt", add_func);
        clock_gettime(CLOCK_MONOTONIC, &t2);
        pipes_t = elapsed(&t1, &t2);

        // Mmap
        clock_gettime(CLOCK_MONOTONIC, &t1);
        mm = mmap_compute(N_PROC, "temp.txt", add_func);
        clock_gettime(CLOCK_MONOTONIC, &t2);
        mmap_t = elapsed(&t1, &t2);

        // Threads
        clock_gettime(CLOCK_MONOTONIC, &t1);
        thr = threads_compute(N_THREADS, "temp.txt", add_func);
        clock_gettime(CLOCK_MONOTONIC, &t2);
        threads_t = elapsed(&t1, &t2);

        // Write to CSV
        fprintf(fp, "%ld,%.6f,%.6f,%.6f,%.6f\n", N, seq_t, pipes_t, mmap_t, threads_t);
        printf("Processed N=%ld\n", N);

        N = (long)(N * 1.3); // Smooth increase (30% each step)
    }

    fclose(fp);
    printf("✅ Results saved to results.csv\n");
    return 0;
}
