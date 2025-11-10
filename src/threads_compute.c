#include "utils.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct {
    int *numbers;
    size_t start, end;
    unsigned long (*func)(int, int);
    unsigned long result;
} ThreadTask;

void *thread_worker(void *arg) {
    ThreadTask *task = (ThreadTask *)arg;
    if (task->start >= task->end) return NULL;
    task->result = task->numbers[task->start];
    for (size_t i = task->start + 1; i < task->end; ++i)
        task->result = task->func(task->result, task->numbers[i]);
    return NULL;
}

unsigned long threads_compute(int n_threads, const char *path, unsigned long (*func)(int, int)) {
    size_t count = 0;
    int *numbers = load_numbers(path, &count);
    if (!numbers || count == 0 || n_threads < 1) return 0;
    pthread_t *threads = malloc(n_threads * sizeof(pthread_t));
    ThreadTask *tasks = malloc(n_threads * sizeof(ThreadTask));
    size_t chunk = count / n_threads, rem = count % n_threads;
    for (int i = 0; i < n_threads; ++i) {
        size_t start = i * chunk + (i < rem ? i : rem);
        size_t end = start + chunk + (i < rem ? 1 : 0);
        tasks[i] = (ThreadTask){numbers, start, end, func, 0};
        pthread_create(&threads[i], NULL, thread_worker, &tasks[i]);
    }
    for (int i = 0; i < n_threads; ++i) pthread_join(threads[i], NULL);
    unsigned long result = tasks[0].result;
    for (int i = 1; i < n_threads; ++i) result = func(result, tasks[i].result);
    free(numbers);
    free(threads);
    free(tasks);
    return result;
}
