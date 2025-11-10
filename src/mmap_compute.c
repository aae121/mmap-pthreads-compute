#define _GNU_SOURCE
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

int mmap_compute(int n_proc, const char *path, int (*func)(int, int)) {
    size_t count = 0;
    int *numbers = load_numbers(path, &count);
    if (!numbers || count == 0 || n_proc < 1) return 0;
    if (n_proc > (int)count) n_proc = (int)count;
    int *partials = mmap(NULL, n_proc * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (partials == MAP_FAILED) {
        perror("mmap");
        free(numbers);
        return 0;
    }
    int chunk = count / n_proc;
    pid_t pid;
    for (int i = 0; i < n_proc; i++) {
        pid = fork();
        if (pid == 0) {
            int start = i * chunk;
            int end = (i == n_proc - 1) ? (int)count : start + chunk;
            int partial = numbers[start];
            for (int j = start + 1; j < end; j++)
                partial = func(partial, numbers[j]);
            partials[i] = partial;
            munmap(partials, n_proc * sizeof(int));
            free(numbers);
            exit(0);
        }
    }
    int result, partial;
    wait(NULL); // wait for at least one child
    result = partials[0];
    for (int i = 1; i < n_proc; i++) {
        wait(NULL);
        partial = partials[i];
        result = func(result, partial);
    }
    munmap(partials, n_proc * sizeof(int));
    free(numbers);
    return result;
}
