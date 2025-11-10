#define _GNU_SOURCE
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

unsigned long mmap_compute(int n_proc, const char *path, unsigned long (*func)(int, int)) {
    size_t count = 0;
    int *numbers = load_numbers(path, &count);
    if (!numbers || count == 0 || n_proc < 1) return 0;
    
    unsigned long *partials = mmap(NULL, n_proc * sizeof(unsigned long), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (partials == MAP_FAILED) {
        perror("mmap");
        free(numbers);
        return 0;
    }
    size_t chunk = count / n_proc, rem = count % n_proc;
    pid_t *pids = malloc(n_proc * sizeof(pid_t));
    for (int i = 0; i < n_proc; ++i) {
        size_t start = i * chunk + (i < rem ? i : rem);
        size_t end = start + chunk + (i < rem ? 1 : 0);
        pids[i] = fork();
        if (pids[i] == 0) {
            unsigned long part = numbers[start];
            for (size_t j = start + 1; j < end; ++j) part = func(part, numbers[j]);
            partials[i] = part;
            munmap(partials, n_proc * sizeof(unsigned long));
            free(numbers);
            exit(0);
        }
    }
    for (int i = 0; i < n_proc; ++i) waitpid(pids[i], NULL, 0);
    unsigned long result = partials[0];
    for (int i = 1; i < n_proc; ++i) result = func(result, partials[i]);
    munmap(partials, n_proc * sizeof(unsigned long));
    free(numbers);
    free(pids);
    return result;
}
