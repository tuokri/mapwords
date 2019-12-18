#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include "hash.h"

hash_t
hash_djb2(const char* buffer)
{
#ifdef DEBUG
    assert(buffer != NULL);
#endif

    uint64_t n = strlen(buffer);
    hash_t hash = 5381;

    for (uint64_t i = 0; i < n; ++i)
    {
        // hash * 33 + c
        hash = ((hash << 5U) + hash) + buffer[i];
    }

    return hash;
}

hash_t
hash_sdbm(const char* buffer)
{
#ifdef DEBUG
    assert(buffer != NULL);
#endif

    uint64_t n = strlen(buffer);
    hash_t hash = 0;

    for (uint64_t i = 0; i < n; ++i)
    {
        hash = buffer[i] + (hash << 6U) + (hash << 16U) - hash;
    }

    return hash;
}

hash_t
hash_java(const char* buffer)
{
#ifdef DEBUG
    assert(buffer != NULL);
#endif

    hash_t hash = 0;
    uint64_t n = strlen(buffer);

    for (uint64_t i = 0; i < n; ++i)
    {
        hash += buffer[i] * pow(31, n - i);
    }

    return hash;
}

hash_t (* get_hashf(const char* hashf_name))(const char*)
{
    if (strcmp(hashf_name, "hash_djb2") == 0)
    {
        return hash_djb2;
    } else if (strcmp(hashf_name, "hash_java") == 0)
    {
        return hash_java;
    } else if (strcmp(hashf_name, "hash_sdbm") == 0)
    {
        return hash_sdbm;
    } else
    {
        return NULL;
    }
}
