#include "compute.h"
#include "utils.h"
#include <stdio.h>
#include <time.h>

#define DATA_PATH "data/numbers.txt"
#define N_PROC 4
#define N_THREADS 4

static double elapsed(struct timespec *start, struct timespec *end) {
    return (end->tv_sec - start->tv_sec) + (end->tv_nsec - start->tv_nsec) / 1e9;
}

int main() {
    struct timespec t1, t2;
    unsigned long res;

    printf("Sequential: ");
    clock_gettime(CLOCK_MONOTONIC, &t1);
    res = sequential_compute(DATA_PATH, add_func);
    clock_gettime(CLOCK_MONOTONIC, &t2);
    printf("%lu (%.6f s)\n", res, elapsed(&t1, &t2));

    printf("mmap (%d proc): ", N_PROC);
    clock_gettime(CLOCK_MONOTONIC, &t1);
    res = mmap_compute(N_PROC, DATA_PATH, add_func);
    clock_gettime(CLOCK_MONOTONIC, &t2);
    printf("%lu (%.6f s)\n", res, elapsed(&t1, &t2));

    printf("threads (%d): ", N_THREADS);
    clock_gettime(CLOCK_MONOTONIC, &t1);
    res = threads_compute(N_THREADS, DATA_PATH, add_func);
    clock_gettime(CLOCK_MONOTONIC, &t2);
    printf("%lu (%.6f s)\n", res, elapsed(&t1, &t2));

    return 0;
}
