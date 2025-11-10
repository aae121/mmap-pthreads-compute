#include "compute.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct {
    int *numbers;
    size_t start;
    size_t end;
    int (*func)(int, int);
    int result;
} ThreadTask;

static void *thread_worker(void *arg) {
    ThreadTask *task = (ThreadTask *)arg;
    if (!task || !task->func || task->start >= task->end) {
        task->result = 0;
        return NULL;
    }

    int acc = task->numbers[task->start];
    for (size_t i = task->start + 1; i < task->end; ++i) {
        acc = task->func(acc, task->numbers[i]);
    }
    task->result = acc;
    return NULL;
}

int threads_compute(int n_threads, const char *path, int (*func)(int, int)) {
    if (n_threads <= 0 || !func) return 0;

    size_t count = 0;
    int *numbers = load_numbers(path, &count);
    if (!numbers || count == 0) {
        if (numbers) free(numbers);
        return 0;
    }

    if (n_threads > (int)count) n_threads = (int)count;

    pthread_t *threads = malloc((size_t)n_threads * sizeof(*threads));
    ThreadTask *tasks = malloc((size_t)n_threads * sizeof(*tasks));
    if (!threads || !tasks) {
        perror("malloc");
        free(numbers);
        free(threads);
        free(tasks);
        return 0;
    }

    size_t base = count / n_threads;
    size_t rem  = count % n_threads;
    size_t start = 0;

    for (int i = 0; i < n_threads; ++i) {
        size_t len = base + (((size_t)i < rem) ? 1u : 0u);
        tasks[i].numbers = numbers;
        tasks[i].start   = start;
        tasks[i].end     = start + len;
        tasks[i].func    = func;
        tasks[i].result  = 0;

        if (pthread_create(&threads[i], NULL, thread_worker, &tasks[i]) != 0) {
            perror("pthread_create");
            n_threads = i;
            break;
        }
        start += len;
    }

    for (int i = 0; i < n_threads; ++i) {
        pthread_join(threads[i], NULL);
    }

    int total = (n_threads > 0) ? tasks[0].result : 0;
    for (int i = 1; i < n_threads; ++i) {
        total = func(total, tasks[i].result);
    }

    free(threads);
    free(tasks);
    free(numbers);
    return total;
}
