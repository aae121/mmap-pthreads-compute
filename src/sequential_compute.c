#include "compute.h"
#include <stdlib.h>

int sequential_compute(const char *path, int (*func)(int, int)) {
    size_t count = 0;
    int *numbers = load_numbers(path, &count);
    if (!numbers || count == 0 || !func) {
        if (numbers) free(numbers);
        return 0;
    }

    int result = numbers[0];
    for (size_t i = 1; i < count; ++i) {
        result = func(result, numbers[i]);
    }

    free(numbers);
    return result;
}
