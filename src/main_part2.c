#include "compute.h"
#include "utils.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define DATA_PATH   "data/numbers.txt"
#define MAX_PROC    16       // max number of processes/threads
#define FIXED_N     50000000 // large N for the test

static double elapsed(const struct timespec *a, const struct timespec *b) {
    return (b->tv_sec - a->tv_sec) + (b->tv_nsec - a->tv_nsec) / 1e9;
}

int main(void) {
    int procs[] = {1, 2, 4, 8, 16};
    int num_procs = sizeof(procs) / sizeof(procs[0]);

    // make a fixed-size dataset
    char cmd[128];
    snprintf(cmd, sizeof(cmd), "head -n %d %s > temp.txt", FIXED_N, DATA_PATH);
    system(cmd);

    FILE *fp = fopen("results_part2.csv", "w");
    if (!fp) {
        perror("Error creating results_part2.csv");
        return 1;
    }
    fprintf(fp, "n_proc,Sequential,Pipes,Mmap,Threads\n");

    for (int i = 0; i < num_procs; i++) {
        int nproc = procs[i];
        struct timespec t1, t2;
        double seq_t, pipes_t, mmap_t, threads_t;
        int seq, pipes, mm, thr;

        // Sequential (runs once)
        clock_gettime(CLOCK_MONOTONIC, &t1);
        seq = sequential_compute("temp.txt", add_func);
        clock_gettime(CLOCK_MONOTONIC, &t2);
        seq_t = elapsed(&t1, &t2);

        // Pipes
        clock_gettime(CLOCK_MONOTONIC, &t1);
        pipes = pipes_compute(nproc, "temp.txt", add_func);
        clock_gettime(CLOCK_MONOTONIC, &t2);
        pipes_t = elapsed(&t1, &t2);

        // Mmap
        clock_gettime(CLOCK_MONOTONIC, &t1);
        mm = mmap_compute(nproc, "temp.txt", add_func);
        clock_gettime(CLOCK_MONOTONIC, &t2);
        mmap_t = elapsed(&t1, &t2);

        // Threads
        clock_gettime(CLOCK_MONOTONIC, &t1);
        thr = threads_compute(nproc, "temp.txt", add_func);
        clock_gettime(CLOCK_MONOTONIC, &t2);
        threads_t = elapsed(&t1, &t2);

        fprintf(fp, "%d,%.6f,%.6f,%.6f,%.6f\n",
                nproc, seq_t, pipes_t, mmap_t, threads_t);
        printf("Processed n_proc=%d\n", nproc);
    }

    fclose(fp);
    printf("✅ Results saved to results_part2.csv\n");
    return 0;
}
