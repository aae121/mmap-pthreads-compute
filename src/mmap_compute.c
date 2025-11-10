#define _GNU_SOURCE
#include "compute.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

int mmap_compute(int n_proc, const char *path, int (*func)(int, int)) {
    if (n_proc <= 0 || !func) return 0;

    size_t count = 0;
    int *numbers = load_numbers(path, &count);
    if (!numbers || count == 0) {
        if (numbers) free(numbers);
        return 0;
    }

    if (n_proc > (int)count) n_proc = (int)count;

    int *partials = mmap(NULL,
                         (size_t)n_proc * sizeof(int),
                         PROT_READ | PROT_WRITE,
                         MAP_SHARED | MAP_ANONYMOUS,
                         -1,
                         0);
    if (partials == MAP_FAILED) {
        perror("mmap");
        free(numbers);
        return 0;
    }

    size_t base = count / n_proc;
    size_t rem  = count % n_proc;
    size_t start = 0;

    for (int i = 0; i < n_proc; ++i) {
        size_t len = base + (((size_t)i < rem) ? 1u : 0u);
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            munmap(partials, (size_t)n_proc * sizeof(int));
            free(numbers);
            return 0;
        }
        if (pid == 0) {
            if (len == 0) _exit(0);
            int acc = numbers[start];
            for (size_t j = start + 1; j < start + len; ++j) {
                acc = func(acc, numbers[j]);
            }
            partials[i] = acc;
            _exit(0);
        }
        start += len;
    }

    for (int i = 0; i < n_proc; ++i) {
        wait(NULL);
    }

    int total = partials[0];
    for (int i = 1; i < n_proc; ++i) {
        total = func(total, partials[i]);
    }

    munmap(partials, (size_t)n_proc * sizeof(int));
    free(numbers);
    return total;
}
