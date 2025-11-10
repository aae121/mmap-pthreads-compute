#ifndef COMPUTE_H
#define COMPUTE_H

unsigned long sequential_compute(const char *path, unsigned long (*func)(int, int));
unsigned long pipes_compute(int n_proc, const char *path, unsigned long (*func)(int, int));
unsigned long mmap_compute(int n_proc, const char *path, unsigned long (*func)(int, int));
unsigned long threads_compute(int n_threads, const char *path, unsigned long (*func)(int, int));

#endif // COMPUTE_H
