#ifndef MAPWORDS_UTIL_H
#define MAPWORDS_UTIL_H

#include <stdint.h>

#define SEARCH_NOT_FOUND -1

/**
 * Find location of value in sorted array.
 * @param arr The array to be searched.
 * @param left Left array index boundary. Inclusive.
 * @param right Right array index boundary. Inclusive.
 * @param x The value to be searched.
 * @return Index (position) of x in arr.
 */
int64_t
binary_search(uint32_t* arr, uint32_t left, uint32_t right, uint32_t x);

#endif //MAPWORDS_UTIL_H
