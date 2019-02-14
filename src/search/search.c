#include "search.h"

int64_t binary_search(uint32_t* arr, uint32_t left, uint32_t right, uint32_t x)
{
    while(left <= right)
    {
        uint32_t middle = left + ((right - left) / 2);

        if (arr[middle] == x)
        {
            return middle;
        }

        // If x is greater than middle, only search right half.
        if (x > arr[middle])
        {
            left = middle + 1;
        }
        else
        {
            right = middle - 1;
        }
    }

    return SEARCH_NOT_FOUND;
}
