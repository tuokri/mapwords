#include <stdio.h>
#include <stdlib.h>

#include "search.h"

uint32_t
find_closest(const uint32_t* const arr, uint32_t left, uint32_t right, uint32_t x)
{
    int64_t diff = INT64_MAX;
    int64_t diff_right = 0;
    int64_t diff_left = 0;
    uint32_t closest = 0;

    while(left <= right)
    {
        uint32_t middle = (left + right) / 2;
        printf("middle=%d\n", middle);

        // Respect array boundaries.
        if(middle + 1 <= right)
        {
            diff_right = labs((int64_t)arr[middle + 1] - (int64_t)x);
        }
        if(middle > 0)
        {
            diff_left = labs((int64_t)arr[middle - 1] - (int64_t)x);
        }

        if(diff_left < diff)
        {
            diff = diff_left;
            closest = arr[middle - 1];
        }
        if(diff_right < diff)
        {
            diff = diff_right;
            closest = arr[middle + 1];
        }

        // If x is greater than middle, only search right half.
        if(x > arr[middle])
        {
            left = middle + 1;
        }
        // Likewise, but for left half.
        else if(x < arr[middle])
        {
            right = middle - 1;
        }
        else
        {
            return arr[middle];
        }
    }

    return closest;
}
