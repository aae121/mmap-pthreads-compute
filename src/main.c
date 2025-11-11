#include "compute.h"
#include "utils.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define N_PROC      16
#define N_THREADS   16
#define MAX_N       50000000

static double elapsed(const struct timespec *a, const struct timespec *b) {
    return (b->tv_sec - a->tv_sec) + (b->tv_nsec - a->tv_nsec) / 1e9;
}

int main(void) {
    FILE *fp = fopen("results.csv", "w");
    if (!fp) { perror("Error creating results.csv"); return 1; }

    fprintf(fp, "N,Sequential,Pipes,Mmap,Threads\n");

    long N = 10000;
    while (N <= MAX_N) {
        // Create an array of N numbers in memory
        int *arr = malloc(N * sizeof(int));
        if (!arr) { perror("malloc failed"); return 1; }
        for (long i = 0; i < N; i++)
            arr[i] = i + 1; // simple increasing sequence

        struct timespec t1, t2;
        double seq_t, pipes_t, mmap_t, threads_t;

        // Sequential
        clock_gettime(CLOCK_MONOTONIC, &t1);
        unsigned long seq = sequential_compute_array(arr, N, add_func);
        clock_gettime(CLOCK_MONOTONIC, &t2);
        seq_t = elapsed(&t1, &t2);

        // Pipes (simulated in-memory variant)
        clock_gettime(CLOCK_MONOTONIC, &t1);
        unsigned long pipes = pipes_compute_array(N_PROC, arr, N, add_func);
        clock_gettime(CLOCK_MONOTONIC, &t2);
        pipes_t = elapsed(&t1, &t2);

        // Mmap (simulated shared memory variant)
        clock_gettime(CLOCK_MONOTONIC, &t1);
        unsigned long mm = mmap_compute_array(N_PROC, arr, N, add_func);
        clock_gettime(CLOCK_MONOTONIC, &t2);
        mmap_t = elapsed(&t1, &t2);

        // Threads
        clock_gettime(CLOCK_MONOTONIC, &t1);
        unsigned long thr = threads_compute_array(N_THREADS, arr, N, add_func);
        clock_gettime(CLOCK_MONOTONIC, &t2);
        threads_t = elapsed(&t1, &t2);

        fprintf(fp, "%ld,%.6f,%.6f,%.6f,%.6f\n", N, seq_t, pipes_t, mmap_t, threads_t);
        printf("Processed N = %ld\n", N);

        free(arr);
        N = (long)(N * 1.3);
    }

    fclose(fp);
    printf("✅ Results saved to results.csv\n");
    return 0;
}