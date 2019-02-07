#include <stdint.h>

#include "hash.h"

hash_t
hash_djb2(char *str)
{
    hash_t hash = 5381;
    int8_t c;

    while((c = *str++))
    {
        // hash * 33 + c
        hash = ((hash << 5) + hash) + c;
    }

    return hash;
}

hash_t
hash_sdbm(char *str)
{
    hash_t hash = 0;
    int8_t c;

    while((c = *str++))
    {
        hash = c + (hash << 6) + (hash << 16) - hash;
    }

    return hash;
}
