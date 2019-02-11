#ifndef MAPWORDS_HASHMAP_H
#define MAPWORDS_HASHMAP_H

#include <stddef.h>

#include "hash.h"

// TODO:
// Choose initial size for map.
// Need a prime table for resizing?
// Store arbitrary data in hashmap with void*?

typedef struct hashmap_element
{
    // store key??
    void* data; // Use int for word counting?
    size_t size;
} element_t;

typedef struct hashmap
{
    size_t size;
    hash_t (*hash1)(char* buffer, size_t size);
    hash_t (*hash2)(char* buffer, size_t size);
    element_t* elements;
} hashmap_t;

hashmap_t*
init_hashmap(
    hash_t (*hash1)(char* buffer, size_t size),
    hash_t (*hash2)(char* buffer, size_t size));

// TODO:
// hashmap_get(mymap, "kukko"):
//   *mymap->elements[mymap->hash("kukko") % size_t] (modulo now or later?)

// TODO:
// hashmap_add(mymap, "kukko");
//   mymap->elements[mymap->hash("kukko") % size_t] = data (AKA count here)

#endif //MAPWORDS_HASHMAP_H
