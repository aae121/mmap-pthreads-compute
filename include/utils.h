#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>

/* Load all whitespace-separated integers from `path` into a heap array.
 * On success:
 *   - returns pointer to array (must be freed by caller)
 *   - writes number of elements into *count
 * On failure:
 *   - returns NULL and sets *count = 0
 */
int *load_numbers(const char *path, size_t *count);

/* Associative operations usable with the compute functions. */
int add_func(int a, int b);
int max_func(int a, int b);

#endif // UTILS_H
