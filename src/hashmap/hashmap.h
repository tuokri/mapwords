#ifndef MAPWORDS_HASHMAP_H
#define MAPWORDS_HASHMAP_H

#include "hash.h"

// TODO:
// Choose initial size for map.
// Need a prime table for resizing?
// Store arbitrary data in hashmap with void*?

typedef struct hashmap
{
    hash_t key;
    uint32_t value;
    hash_t (*hash1)(char* buffer, size_t size);
    hash_t (*hash2)(char* buffer, size_t size);
} hashmap_t;

hashmap_t*
init_hashmap
(
    size_t init_size,
    hash_t (*hash1)(char* buffer, size_t size),
    hash_t (*hash2)(char* buffer, size_t size)
);

#endif //MAPWORDS_HASHMAP_H
