#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

int *load_numbers(const char *path, size_t *count) {
    FILE *f = fopen(path, "r");
    if (!f) {
        fprintf(stderr, "Error: Could not open file '%s'\n", path);
        *count = 0;
        exit(EXIT_FAILURE);
    }
    size_t cap = 128, n = 0;
    int *arr = malloc(cap * sizeof(int));
    if (!arr) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        fclose(f);
        *count = 0;
        exit(EXIT_FAILURE);
    }
    int tmp;
    while (fscanf(f, "%d", &tmp) == 1) {
        if (n == cap) {
            cap *= 2;
            int *new_arr = realloc(arr, cap * sizeof(int));
            if (!new_arr) {
                fprintf(stderr, "Error: Memory allocation failed\n");
                free(arr);
                fclose(f);
                *count = 0;
                exit(EXIT_FAILURE);
            }
            arr = new_arr;
        }
        arr[n++] = tmp;
    }
    fclose(f);
    if (n == 0) {
        fprintf(stderr, "Error: No valid numbers found in file '%s'\n", path);
        free(arr);
        *count = 0;
        exit(EXIT_FAILURE);
    }
    *count = n;
    return arr;
}

unsigned long add_func(int a, int b) { return (unsigned long)a + b; }
unsigned long max_func(int a, int b) { return a > b ? a : b; }
