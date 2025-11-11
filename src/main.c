#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "compute.h"
#include "utils.h"

/* Print usage information. */
static void usage(const char *prog) {
    fprintf(stderr,
        "Usage:\n"
        "  %s seq      <sum|max> <file>\n"
        "  %s pipes    <sum|max> <file> <n_proc>\n"
        "  %s mmap     <sum|max> <file> <n_proc>\n"
        "  %s threads  <sum|max> <file> <n_threads>\n"
        "  %s bench    <file>\n"
        "\n"
        "Examples:\n"
        "  %s seq sum numbers.txt\n"
        "  %s pipes sum numbers.txt 4\n"
        "  %s mmap max numbers.txt 8\n"
        "  %s threads sum numbers.txt 8\n"
        "  %s bench numbers.txt > results.csv\n",
        prog, prog, prog, prog, prog,
        prog, prog, prog, prog, prog);
}

/* Map operation name to function pointer. */
static int (*get_op(const char *name))(int, int) {
    if (strcmp(name, "sum") == 0 || strcmp(name, "add") == 0)
        return add_func;
    if (strcmp(name, "max") == 0)
        return max_func;
    return NULL;
}

/* Elapsed time helper (ms). */
static double elapsed_ms(const struct timespec *start,
                         const struct timespec *end) {
    return (end->tv_sec - start->tv_sec) * 1000.0
         + (end->tv_nsec - start->tv_nsec) / 1e6;
}

/* Timed wrapper:
 * mode: 0=seq, 1=pipes, 2=mmap, 3=threads
 * workers: used only for modes that need it.
 */
static int timed_compute(int mode, int workers,
                         const char *path,
                         int (*func)(int, int),
                         double *ms_out) {
    struct timespec t1, t2;
    int result = 0;

    if (clock_gettime(CLOCK_MONOTONIC, &t1) != 0) {
        perror("clock_gettime");
        return 0;
    }

    switch (mode) {
        case 0:
            result = sequential_compute(path, func);
            break;
        case 1:
            result = pipes_compute(workers, path, func);
            break;
        case 2:
            result = mmap_compute(workers, path, func);
            break;
        case 3:
            result = threads_compute(workers, path, func);
            break;
        default:
            fprintf(stderr, "timed_compute: invalid mode %d\n", mode);
            return 0;
    }

    if (clock_gettime(CLOCK_MONOTONIC, &t2) != 0) {
        perror("clock_gettime");
        return 0;
    }

    if (ms_out)
        *ms_out = elapsed_ms(&t1, &t2);

    return result;
}

/*
 * Benchmark:
 *  - uses add_func (sum) assuming positive integers in numbers.txt
 *  - prints CSV: method,workers,time_ms,result
 *  - suitable for plotting a performance graph
 */
static int run_bench(const char *path) {
    const int workers_list[] = {1, 2, 4, 8};
    const size_t n_workers = sizeof(workers_list) / sizeof(workers_list[0]);
    int (*func)(int, int) = add_func; /* sum */

    printf("#method,workers,time_ms,result\n");

    /* Baseline: sequential (workers = 1) */
    double ms = 0.0;
    int seq_res = timed_compute(0, 1, path, func, &ms);
    if (seq_res == 0) {
        fprintf(stderr,
                "sequential_compute failed (result 0) in bench; "
                "ensure numbers.txt has positive integers.\n");
        return 0;
    }
    printf("sequential,1,%.3f,%d\n", ms, seq_res);

    /* Parallel methods. Skip clear failures (result == 0). */
    for (size_t i = 0; i < n_workers; ++i) {
        int w = workers_list[i];

        int r_pipes = timed_compute(1, w, path, func, &ms);
        if (r_pipes != 0)
            printf("pipes,%d,%.3f,%d\n", w, ms, r_pipes);

        int r_mmap = timed_compute(2, w, path, func, &ms);
        if (r_mmap != 0)
            printf("mmap,%d,%.3f,%d\n", w, ms, r_mmap);

        int r_thr = timed_compute(3, w, path, func, &ms);
        if (r_thr != 0)
            printf("threads,%d,%.3f,%d\n", w, ms, r_thr);
    }

    return 1;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    const char *mode = argv[1];

    /* Benchmark mode: ./compute bench numbers.txt */
    if (strcmp(mode, "bench") == 0) {
        if (argc != 3) {
            usage(argv[0]);
            return EXIT_FAILURE;
        }
        if (!run_bench(argv[2])) {
            fprintf(stderr, "Benchmark failed.\n");
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }

    if (argc < 4) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    const char *op_name = argv[2];
    const char *path    = argv[3];
    int (*op)(int, int) = get_op(op_name);

    if (!op) {
        fprintf(stderr, "Unknown operation '%s' (use 'sum' or 'max').\n", op_name);
        return EXIT_FAILURE;
    }

    int result = 0;

    if (strcmp(mode, "seq") == 0 || strcmp(mode, "sequential") == 0) {
        result = sequential_compute(path, op);
    } else if (strcmp(mode, "pipes") == 0) {
        if (argc < 5) {
            fprintf(stderr, "Missing n_proc for pipes mode.\n");
            return EXIT_FAILURE;
        }
        int n_proc = atoi(argv[4]);
        result = pipes_compute(n_proc, path, op);
    } else if (strcmp(mode, "mmap") == 0) {
        if (argc < 5) {
            fprintf(stderr, "Missing n_proc for mmap mode.\n");
            return EXIT_FAILURE;
        }
        int n_proc = atoi(argv[4]);
        result = mmap_compute(n_proc, path, op);
    } else if (strcmp(mode, "threads") == 0) {
        if (argc < 5) {
            fprintf(stderr, "Missing n_threads for threads mode.\n");
            return EXIT_FAILURE;
        }
        int n_thr = atoi(argv[4]);
        result = threads_compute(n_thr, path, op);
    } else {
        fprintf(stderr, "Unknown mode '%s'.\n", mode);
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    /* With the given API, 0 can be a valid result or an error;
       we just print it and let the caller interpret. */
    printf("%d\n", result);
    return EXIT_SUCCESS;
}
