#include <stdint.h>
#include <x86intrin.h>

#include "hash.h"

hash_t
hash_djb2(char* buffer, size_t size)
{
    hash_t hash = 5381;

    for(size_t i = 0; i < size; ++i)
    {
        // hash * 33 + buffer[i]
        hash = ((hash << 5) + hash) + buffer[i];
    }

    return hash;
}

hash_t
hash_sdbm(char* buffer, size_t size)
{
    hash_t hash = 0;

    for(size_t i = 0; i < size; ++i)
    {
        hash = buffer[i] + (hash << 6) + (hash << 16) - hash;
    }

    return hash;
}

hash_t
hash_sse42_crc32(char* buffer, size_t size)
{
    hash_t hash = 0;

    for(size_t i = 0; i < size; ++i)
    {
        hash = _mm_crc32_u64(hash, buffer[i]);
    }

    return hash;
};
