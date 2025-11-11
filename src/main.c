#include "compute.h"
#include "utils.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define DATA_PATH   "data/numbers.txt"

// Helper function to measure elapsed time
static double elapsed(const struct timespec *a, const struct timespec *b) {
    return (b->tv_sec - a->tv_sec) + (b->tv_nsec - a->tv_nsec) / 1e9;
}

void write_numbers_space(const char *path) {
    FILE *fp = fopen(path, "w");
    if (!fp) {
        perror("Error opening file");
        exit(1);
    }

    for (long i = 1; i <= 1000000; i++) {
        fprintf(fp, "%ld", i);        // write the number
        if (i < 1000000) fprintf(fp, " "); // add a space except after the last number
    }

    fclose(fp);
    printf("✅ Numbers 1–1,000,000 written to %s (space-separated)\n", path);
}

int main(void) {
    write_numbers_space(DATA_PATH);
    FILE *fp = fopen("results.csv", "w");
    if (!fp) {
        perror("Error creating results.csv");
        return 1;
    }

    fprintf(fp, "N,Sequential,Pipes,Mmap,Threads\n");

    long Number = 1;  // Start small
    while (Number <= 100) {  // Go up to 50 million
        char cmd[2048];
        snprintf(cmd, sizeof(cmd), "head -n %ld %s > temp.txt", 1L, DATA_PATH);

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
        pipes = pipes_compute(Number, "temp.txt", add_func);
        clock_gettime(CLOCK_MONOTONIC, &t2);
        pipes_t = elapsed(&t1, &t2);

        // Mmap
        clock_gettime(CLOCK_MONOTONIC, &t1);
        mm = mmap_compute(Number, "temp.txt", add_func);
        clock_gettime(CLOCK_MONOTONIC, &t2);
        mmap_t = elapsed(&t1, &t2);

        // Threads
        clock_gettime(CLOCK_MONOTONIC, &t1);
        thr = threads_compute(Number, "temp.txt", add_func);
        clock_gettime(CLOCK_MONOTONIC, &t2);
        threads_t = elapsed(&t1, &t2);

        // Write to CSV
        fprintf(fp, "%ld,%.6f,%.6f,%.6f,%.6f\n", Number, seq_t, pipes_t, mmap_t, threads_t);
        printf("Processed number of processes=%ld\n", Number);

        Number = (Number+1); // Smooth increase (30% each step)
    }

    fclose(fp);
    printf("✅ Results saved to results.csv\n");
    return 0;
}