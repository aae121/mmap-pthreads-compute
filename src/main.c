#include "compute.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define DATA_PATH   "data/numbers.txt"
#define N_PROC      8
#define N_THREADS   8
#define MAX_N       50000000

static double elapsed(struct timespec *a, struct timespec *b) {
    return (b->tv_sec - a->tv_sec) + (b->tv_nsec - a->tv_nsec) / 1e9;
}

static void write_subset(const char *src_path, const char *dst_path, long N) {
    FILE *src = fopen(src_path, "r");
    if (!src) { perror("source"); exit(1); }

    FILE *dst = fopen(dst_path, "w");
    if (!dst) { perror("dst"); exit(1); }

    long count = 0;
    int num;

    while (count < N && fscanf(src, "%d", &num) == 1) {
        fprintf(dst, "%d\n", num);
        count++;
    }

    fclose(src);
    fclose(dst);

    if (count < N) {
        fprintf(stderr,
            "ERROR: Input file has only %ld numbers but requested N=%ld\n",
            count, N
        );
        exit(1);
    }
}

int main(void) {
    FILE *fp = fopen("results.csv", "w");
    if (!fp) {
        perror("results.csv");
        exit(1);
    }

    fprintf(fp, "N,Sequential,Pipes,Mmap,Threads\n");

    long N = 10000;

    while (N <= MAX_N) {

        char tempname[64];
        sprintf(tempname, "subset_%ld.txt", N);

        write_subset(DATA_PATH, tempname, N);

        struct timespec t1, t2;
        double t_seq, t_pipe, t_mmap, t_thr;

        // SEQUENTIAL
        clock_gettime(CLOCK_MONOTONIC, &t1);
        sequential_compute(tempname, add_func);
        clock_gettime(CLOCK_MONOTONIC, &t2);
        t_seq = elapsed(&t1, &t2);

        // PIPES
        clock_gettime(CLOCK_MONOTONIC, &t1);
        pipes_compute(N_PROC, tempname, add_func);
        clock_gettime(CLOCK_MONOTONIC, &t2);
        t_pipe = elapsed(&t1, &t2);

        // MMAP
        clock_gettime(CLOCK_MONOTONIC, &t1);
        mmap_compute(N_PROC, tempname, add_func);
        clock_gettime(CLOCK_MONOTONIC, &t2);
        t_mmap = elapsed(&t1, &t2);

        // THREADS
        clock_gettime(CLOCK_MONOTONIC, &t1);
        threads_compute(N_THREADS, tempname, add_func);
        clock_gettime(CLOCK_MONOTONIC, &t2);
        t_thr = elapsed(&t1, &t2);

        fprintf(fp, "%ld,%.6f,%.6f,%.6f,%.6f\n",
                N, t_seq, t_pipe, t_mmap, t_thr);

        printf("N = %ld done.\n", N);

        N = (long)(N * 1.35);   // smoothly increase N
    }

    fclose(fp);
    printf("Benchmark complete! Results saved to results.csv\n");
    return 0;
}