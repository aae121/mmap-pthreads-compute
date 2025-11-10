#include "compute.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h> // for sysconf

#define DATA_PATH "data/numbers.txt"

// Function to compute elapsed seconds
static double elapsed(const struct timespec *a, const struct timespec *b) {
    return (b->tv_sec - a->tv_sec) + (b->tv_nsec - a->tv_nsec) / 1e9;
}

// Median smoothing for stable curve
double median(double *arr, int n) {
    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n - i - 1; j++)
            if (arr[j] > arr[j + 1]) {
                double tmp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = tmp;
            }
    return arr[n / 2];
}

// CSV logger
void log_result_to_csv(const char *method, int n, double time, unsigned long result) {
    FILE *fp = fopen("performance_data.csv", "a");
    if (fp) {
        fprintf(fp, "%d,%s,%.6f,%lu\n", n, method, time, result);
        fclose(fp);
    }
}

int main(void) {
    // Auto-detect available cores
    int N_CORES = sysconf(_SC_NPROCESSORS_ONLN);
    int N_PROC = N_CORES;
    int N_THREADS = N_CORES;

    printf("🧠 Using %d CPU cores for parallel computation\n", N_CORES);

    int sizes[] = {1000, 5000, 10000, 20000, 50000, 100000, 200000, 500000, 1000000, 2000000, 5000000};
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);

    FILE *fp = fopen("performance_data.csv", "w");
    if (fp) {
        fprintf(fp, "input_size,method,time_seconds,result\n");
        fclose(fp);
    }

    struct timespec t1, t2;

    // Loop through all sizes
    for (int i = 0; i < num_sizes; i++) {
        int n = sizes[i];
        printf("\n📊 Testing input size = %d\n", n);

        char cmd[256];
        sprintf(cmd, "mkdir -p data && seq 1 %d > %s", n, DATA_PATH);
        system(cmd);

        // Repeat tests multiple times for median smoothing
        int runs = 5;
        double times_seq[runs], times_pipes[runs], times_mmap[runs], times_threads[runs];
        unsigned long result = 0;

        for (int r = 0; r < runs; r++) {
            // Sequential
            clock_gettime(CLOCK_MONOTONIC, &t1);
            result = sequential_compute(DATA_PATH, add_func);
            clock_gettime(CLOCK_MONOTONIC, &t2);
            times_seq[r] = elapsed(&t1, &t2);

            // Pipes
            clock_gettime(CLOCK_MONOTONIC, &t1);
            pipes_compute(N_PROC, DATA_PATH, add_func);
            clock_gettime(CLOCK_MONOTONIC, &t2);
            times_pipes[r] = elapsed(&t1, &t2);

            // mmap
            clock_gettime(CLOCK_MONOTONIC, &t1);
            mmap_compute(N_PROC, DATA_PATH, add_func);
            clock_gettime(CLOCK_MONOTONIC, &t2);
            times_mmap[r] = elapsed(&t1, &t2);

            // Threads
            clock_gettime(CLOCK_MONOTONIC, &t1);
            threads_compute(N_THREADS, DATA_PATH, add_func);
            clock_gettime(CLOCK_MONOTONIC, &t2);
            times_threads[r] = elapsed(&t1, &t2);
        }

        double t_seq = median(times_seq, runs);
        double t_pipes = median(times_pipes, runs);
        double t_mmap = median(times_mmap, runs);
        double t_threads = median(times_threads, runs);

        printf("✅ Sequential: %.6f s | Pipes: %.6f s | mmap: %.6f s | Threads: %.6f s\n",
               t_seq, t_pipes, t_mmap, t_threads);

        log_result_to_csv("sequential", n, t_seq, result);
        log_result_to_csv("pipes", n, t_pipes, result);
        log_result_to_csv("mmap", n, t_mmap, result);
        log_result_to_csv("threads", n, t_threads, result);
    }

    printf("\n✅ All results saved to performance_data.csv\n");
    return 0;
}
