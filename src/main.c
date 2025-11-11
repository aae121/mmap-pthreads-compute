#include "compute.h"
#include "utils.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

// === Configuration ===
#define DATA_PATH   "data/numbers.txt"
#define N_PROC      16
#define N_THREADS   16
#define MAX_N       50000000   // 50 million max for testing

// Helper function to measure elapsed time
static double elapsed(const struct timespec *a, const struct timespec *b) {
    return (b->tv_sec - a->tv_sec) + (b->tv_nsec - a->tv_nsec) / 1e9;
}

// Generate a temporary subset of the input file with first N numbers
static void generate_subset(const char *src_path, const char *dst_path, long N) {
    FILE *src = fopen(src_path, "r");
    if (!src) {
        perror("Error opening source file");
        exit(1);
    }
    FILE *dst = fopen(dst_path, "w");
    if (!dst) {
        perror("Error creating temp file");
        fclose(src);
        exit(1);
    }

    long count = 0;
    int num;
    while (count < N && fscanf(src, "%d", &num) == 1) {
        fprintf(dst, "%d\n", num);
        count++;
    }

    fclose(src);
    fclose(dst);
}

int main(void) {
    FILE *fp = fopen("results.csv", "w");
    if (!fp) {
        perror("Error creating results.csv");
        return 1;
    }

    fprintf(fp, "N,Sequential,Pipes,Mmap,Threads\n");

    long N = 10000;  // Start small and increase progressively
    while (N <= MAX_N) {
        generate_subset(DATA_PATH, "temp.txt", N);

        struct timespec t1, t2;
        double seq_t, pipes_t, mmap_t, threads_t;
        unsigned long seq, pipes, mm, thr;

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
        printf("✅ Processed N = %ld\n", N);

        N = (long)(N * 1.1); // Increase by ~30% each step
    }

    fclose(fp);
    printf("✅ Results saved to results.csv\n");
    return 0;
}