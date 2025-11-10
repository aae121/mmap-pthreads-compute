#include "compute.h"
#include "utils.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define DATA_PATH   "data/numbers.txt"
#define N_PROC      4
#define N_THREADS   4

static double elapsed(const struct timespec *a, const struct timespec *b) {
    return (b->tv_sec - a->tv_sec) + (b->tv_nsec - a->tv_nsec) / 1e9;
}

int main(void) {
    int sizes[] = {1000, 2000, 5000, 10000, 20000, 40000, 80000, 160000, 320000, 640000, 1000000};
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);

    FILE *fp = fopen("results.csv", "w");
    fprintf(fp, "N,Sequential,Pipes,Mmap,Threads\n");

    for (int s = 0; s < num_sizes; s++) {
        char cmd[128];
        snprintf(cmd, sizeof(cmd), "head -n %d %s > temp.txt", sizes[s], DATA_PATH);
        system(cmd);

        struct timespec t1, t2;
        double seq_t, pipes_t, mmap_t, threads_t;
        int seq, pipes, mm, thr;

        clock_gettime(CLOCK_MONOTONIC, &t1);
        seq = sequential_compute("temp.txt", add_func);
        clock_gettime(CLOCK_MONOTONIC, &t2);
        seq_t = elapsed(&t1, &t2);

        clock_gettime(CLOCK_MONOTONIC, &t1);
        pipes = pipes_compute(N_PROC, "temp.txt", add_func);
        clock_gettime(CLOCK_MONOTONIC, &t2);
        pipes_t = elapsed(&t1, &t2);

        clock_gettime(CLOCK_MONOTONIC, &t1);
        mm = mmap_compute(N_PROC, "temp.txt", add_func);
        clock_gettime(CLOCK_MONOTONIC, &t2);
        mmap_t = elapsed(&t1, &t2);

        clock_gettime(CLOCK_MONOTONIC, &t1);
        thr = threads_compute(N_THREADS, "temp.txt", add_func);
        clock_gettime(CLOCK_MONOTONIC, &t2);
        threads_t = elapsed(&t1, &t2);

        fprintf(fp, "%d,%.6f,%.6f,%.6f,%.6f\n", sizes[s], seq_t, pipes_t, mmap_t, threads_t);
        printf("Processed N=%d\n", sizes[s]);
    }

    fclose(fp);
    return 0;
}
