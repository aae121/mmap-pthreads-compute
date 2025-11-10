#include "compute.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int pipes_compute(int n_proc, const char *path, int (*func)(int, int)) {
    if (n_proc <= 0 || !func) return 0;

    size_t count = 0;
    int *numbers = load_numbers(path, &count);
    if (!numbers || count == 0) {
        if (numbers) free(numbers);
        return 0;
    }

    if (n_proc > (int)count) n_proc = (int)count;

    size_t base = count / n_proc;
    size_t rem  = count % n_proc;
    size_t start = 0;

    int total = 0;
    int have_total = 0;

    for (int i = 0; i < n_proc; ++i) {
        int fd[2];
        if (pipe(fd) == -1) {
            perror("pipe");
            free(numbers);
            return 0;
        }

        size_t len = base + (((size_t)i < rem) ? 1u : 0u);
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            close(fd[0]);
            close(fd[1]);
            free(numbers);
            return 0;
        }

        if (pid == 0) {
            // child
            close(fd[0]);
            if (len == 0) _exit(0);
            int acc = numbers[start];
            for (size_t j = start + 1; j < start + len; ++j) {
                acc = func(acc, numbers[j]);
            }
            if (write(fd[1], &acc, sizeof(acc)) != sizeof(acc)) {
                perror("write");
            }
            close(fd[1]);
            _exit(0);
        }

        // parent
        close(fd[1]);
        int partial = 0;
        if (read(fd[0], &partial, sizeof(partial)) == sizeof(partial)) {
            if (!have_total) {
                total = partial;
                have_total = 1;
            } else {
                total = func(total, partial);
            }
        }
        close(fd[0]);
        start += len;
    }

    while (wait(NULL) > 0) {
        /* wait for all children */
    }

    free(numbers);
    return have_total ? total : 0;
}
