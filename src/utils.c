#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

int *load_numbers(const char *path, size_t *count) {
    FILE *fp = fopen(path, "r");
    if (!fp) return NULL;
    int *arr = NULL, n = 0;
    char line[128];
    while (fgets(line, sizeof(line), fp)) {
        char *tok = strtok(line, ", \t\n");
        while (tok) {
            arr = realloc(arr, (n + 1) * sizeof(int));
            arr[n++] = atoi(tok);
            tok = strtok(NULL, ", \t\n");
        }
    }
    fclose(fp);
    *count = n;
    return arr;
}

int add_func(int a, int b) { return a + b; }
unsigned long max_func(int a, int b) { return a > b ? a : b; }
