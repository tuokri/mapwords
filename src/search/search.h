#ifndef MAPWORDS_SEARCH_H
#define MAPWORDS_SEARCH_H

#include <stdint.h>

/**
 * Find value closest to target in array.
 * @param arr The array to be searched.
 * @param left Left array index boundary. Inclusive.
 * @param right Right array index boundary. Inclusive.
 * @param x The value to be searched.
 * @return Closes value to x in arr.
 */
uint32_t
find_closest(const uint32_t* arr, uint32_t left, uint32_t right, uint32_t x);

#endif //MAPWORDS_SEARCH_H
