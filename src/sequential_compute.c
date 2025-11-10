#include "utils.h"
#include <stdio.h>

unsigned long sequential_compute(const char *path, unsigned long (*func)(int, int)) {
    size_t count = 0;
    int *numbers = load_numbers(path, &count);
    if (!numbers || count == 0) return 0;
    unsigned long result = numbers[0];
    for (size_t i = 1; i < count; ++i) {
        result = func(result, numbers[i]);
    }
    free(numbers);
    return result;
}
