#ifndef COMPUTE_H
#define COMPUTE_H

int sequential_compute(const char *path, int (*func)(int, int));
unsigned long pipes_compute(int n_proc, const char *path, unsigned long (*func)(int, int));
int mmap_compute(int n_proc, const char *path, int (*func)(int, int));
unsigned long threads_compute(int n_threads, const char *path, unsigned long (*func)(int, int));

#endif // COMPUTE_H
