#include "compute.h"
#include "utils.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

#define DATA_PATH   "data/numbers.txt"
#define N_PROC      16
#define N_THREADS   16
#define MAX_N       50000000
#define RUNS        5        // average over 5 runs for accuracy

// ---------- helper functions ----------
static double elapsed(const struct timespec *a, const struct timespec *b) {
    return (b->tv_sec - a->tv_sec) + (b->tv_nsec - a->tv_nsec) / 1e9;
}

// Make sure Linux drops disk cache between tests
static void drop_cache() {
    system("sync");
    system("echo 3 > /proc/sys/vm/drop_caches");
}

// Generate a TEMP FILE with the first N numbers from the source file
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

    if (count < N) {
        fprintf(stderr, "❌ ERROR: numbers.txt does NOT contain %ld numbers.\n", N);
        exit(1);
    }
}

// ---------- main benchmarking ----------
int main(void) {
    FILE *fp = fopen("results.csv", "w");
    if (!fp) {
        perror("Error creating results.csv");
        return 1;
    }

    fprintf(fp, "N,Sequential,Pipes,Mmap,Threads\n");

    long N = 10;

    while (N <= MAX_N) {

        char tempname[64];
        sprintf(tempname, "temp_%ld.txt", N);

        generate_subset(DATA_PATH, tempname, N);

        double seq_avg = 0, pipes_avg = 0, mmap_avg = 0, threads_avg = 0;

        for (int r = 0; r < RUNS; r++) {

            struct timespec t1, t2;

            // ---------- SEQUENTIAL ----------
            drop_cache();
            clock_gettime(CLOCK_MONOTONIC, &t1);
            sequential_compute(tempname, add_func);
            clock_gettime(CLOCK_MONOTONIC, &t2);
            seq_avg += elapsed(&t1, &t2);

            // ---------- PIPES ----------
            drop_cache();
            clock_gettime(CLOCK_MONOTONIC, &t1);
            pipes_compute(N_PROC, tempname, add_func);
            clock_gettime(CLOCK_MONOTONIC, &t2);
            pipes_avg += elapsed(&t1, &t2);

            // ---------- MMAP ----------
            drop_cache();
            clock_gettime(CLOCK_MONOTONIC, &t1);
            mmap_compute(N_PROC, tempname, add_func);
            clock_gettime(CLOCK_MONOTONIC, &t2);
            mmap_avg += elapsed(&t1, &t2);

            // ---------- THREADS ----------
            drop_cache();
            clock_gettime(CLOCK_MONOTONIC, &t1);
            threads_compute(N_THREADS, tempname, add_func);
            clock_gettime(CLOCK_MONOTONIC, &t2);
            threads_avg += elapsed(&t1, &t2);
        }

        seq_avg     /= RUNS;
        pipes_avg   /= RUNS;
        mmap_avg    /= RUNS;
        threads_avg /= RUNS;

        fprintf(fp, "%ld,%.6f,%.6f,%.6f,%.6f\n",
                N, seq_avg, pipes_avg, mmap_avg, threads_avg);

        printf("✅ Finished N = %ld\n", N);

        N = (long)(N * 1.3);
    }

    fclose(fp);
    printf("✅ Results saved to results.csv\n");

    return 0;
}