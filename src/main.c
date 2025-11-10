#include "compute.h"
#include "utils.h"
#include <stdio.h>
#include <time.h>

#define DATA_PATH   "data/numbers.txt"
#define N_PROC      4
#define N_THREADS   4

static double elapsed(const struct timespec *a, const struct timespec *b) {
    return (b->tv_sec - a->tv_sec) + (b->tv_nsec - a->tv_nsec) / 1e9;
}

int main(void) {
    struct timespec t1, t2;

    // Sequential
    clock_gettime(CLOCK_MONOTONIC, &t1);
    int seq = sequential_compute(DATA_PATH, add_func);
    clock_gettime(CLOCK_MONOTONIC, &t2);
    printf("sequential: %d (%.6f s)\n", seq, elapsed(&t1, &t2));

    // Pipes (processes)
    clock_gettime(CLOCK_MONOTONIC, &t1);
    int pipes = pipes_compute(N_PROC, DATA_PATH, add_func);
    clock_gettime(CLOCK_MONOTONIC, &t2);
    printf("pipes (%d): %d (%.6f s)\n", N_PROC, pipes, elapsed(&t1, &t2));

    // mmap (shared memory + processes)
    clock_gettime(CLOCK_MONOTONIC, &t1);
    int mm = mmap_compute(N_PROC, DATA_PATH, add_func);
    clock_gettime(CLOCK_MONOTONIC, &t2);
    printf("mmap (%d): %d (%.6f s)\n", N_PROC, mm, elapsed(&t1, &t2));

    // pthreads
    clock_gettime(CLOCK_MONOTONIC, &t1);
    int thr = threads_compute(N_THREADS, DATA_PATH, add_func);
    clock_gettime(CLOCK_MONOTONIC, &t2);
    printf("threads (%d): %d (%.6f s)\n", N_THREADS, thr, elapsed(&t1, &t2));
    FILE *fp = fopen("results.csv", "a");
    if (fp) {
        fprintf(fp, "method,time_s,result\n");
        fprintf(fp, "sequential,%.6f,%d\n", elapsed(&t1, &t2), seq);
        fprintf(fp, "pipes,%.6f,%d\n", elapsed(&t1, &t2), pipes);
        fprintf(fp, "mmap,%.6f,%d\n", elapsed(&t1, &t2), mm);
        fprintf(fp, "threads,%.6f,%d\n", elapsed(&t1, &t2), thr);
        fclose(fp);
    }

    return 0;
}
