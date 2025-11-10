#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

/* Robust loader: uses fscanf to read any whitespace-separated integers. */
int *load_numbers(const char *path, size_t *count) {
    FILE *fp = fopen(path, "r");
    if (!fp) {
        perror("fopen");
        if (count) *count = 0;
        return NULL;
    }

    size_t cap = 1024;
    int *arr = malloc(cap * sizeof(int));
    if (!arr) {
        perror("malloc");
        fclose(fp);
        if (count) *count = 0;
        return NULL;
    }

    size_t n = 0;
    long value;
    while (fscanf(fp, "%ld", &value) == 1) {
        if (n == cap) {
            cap *= 2;
            int *tmp = realloc(arr, cap * sizeof(int));
            if (!tmp) {
                perror("realloc");
                free(arr);
                fclose(fp);
                if (count) *count = 0;
                return NULL;
            }
            arr = tmp;
        }
        arr[n++] = (int)value;
    }

    fclose(fp);

    if (n == 0) {
        free(arr);
        if (count) *count = 0;
        return NULL;
    }

    if (count) *count = n;
    return arr;
}

int add_func(int a, int b) {
    return a + b;
}

int max_func(int a, int b) {
    return (a > b) ? a : b;
}
