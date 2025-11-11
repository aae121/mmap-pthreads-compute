#define MAX_N 50000000
#define RUNS 5
#define N_THREADS 16
#define N_PROC 16

// ---------- main benchmarking ----------
int main(void) {
    FILE *fp = fopen("results.csv", "w");
    if (!fp) {
        perror("Error creating results.csv");
        return 1;
    }

    fprintf(fp, "N,Sequential,Pipes,Mmap,Threads\n");

    long N = 10000; // starting dataset size

    while (N <= MAX_N) {
        char tempname[64];
        sprintf(tempname, "temp_%ld.txt", N);

        // Generate subset of size N
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

        // Average over multiple runs
        seq_avg     /= RUNS;
        pipes_avg   /= RUNS;
        mmap_avg    /= RUNS;
        threads_avg /= RUNS;

        // Write results
        fprintf(fp, "%ld,%.6f,%.6f,%.6f,%.6f\n", N, seq_avg, pipes_avg, mmap_avg, threads_avg);

        printf("✅ Finished N = %ld\n", N);

        N = (long)(N * 1.3); // gradually increase dataset size
    }

    fclose(fp);
    printf("✅ Results saved to results.csv\n");
    return 0;
}
