#ifndef COMPUTE_H
#define COMPUTE_H

#include "utils.h"

/*
 * Sequential, pipes, mmap and threads based computations.
 * Each function:
 *  - reads all ints from `path` using load_numbers()
 *  - partitions the work
 *  - combines values using `func`, which must be associative (e.g. sum)
 * Returns 0 on error or the reduced result otherwise.
 */

int sequential_compute(const char *path, int (*func)(int, int));
int pipes_compute(int n_proc, const char *path, int (*func)(int, int));
int mmap_compute(int n_proc, const char *path, int (*func)(int, int));
int threads_compute(int n_threads, const char *path, int (*func)(int, int));

#endif // COMPUTE_H
#ifndef COMPUTE_H
#define COMPUTE_H

#include "utils.h"

/*
 * Sequential, pipes, mmap and threads based computations.
 * Each function:
 *  - reads all ints from `path` using load_numbers()
 *  - partitions the work
 *  - combines values using `func`, which must be associative (e.g. sum)
 * Returns 0 on error or the reduced result otherwise.
 */

int sequential_compute(const char *path, int (*func)(int, int));
int pipes_compute(int n_proc, const char *path, int (*func)(int, int));
int mmap_compute(int n_proc, const char *path, int (*func)(int, int));
int threads_compute(int n_threads, const char *path, int (*func)(int, int));

#endif // COMPUTE_H
