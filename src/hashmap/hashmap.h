#ifndef MAPWORDS_HASHMAP_H
#define MAPWORDS_HASHMAP_H

#include <stddef.h>
#include <stdbool.h>
#include <inttypes.h>

#include "hash.h"

/*
C string hash map implementation, which stores int64_t as value.
Hash function used by the map can be set at map creation time.

Map uses open addressing. Hash collision are resolved using probing.
Probing sequence algorithm is adapted from CPython dictobject.c.
https://github.com/python/cpython/blob/master/Objects/dictobject.c

Map initial lookup index is calculate as hash % capacity.
Subsequent probe indices i are calculated as:
  perturb >>= PERTURB_SHIFT
  i = (i * 5 + perturb + 1) % map_capacity,
where perturb is initialized as the hash value.
PERTURB_SHIFT value is chosen to be 5. The value may be chosen
arbitrarily, but the CPython choice is also used here.
*/

// Capacity *must* always be a power of two, because
// (hash % capacity) is replaced with (hash & (capacity - 1))
// for performance purposes.
#define HASHMAP_INITIAL_CAPACITY 16U

#define HASHMAP_ERROR -1
#define HASHMAP_OK 0
#define HASHMAP_KEY_NOT_FOUND 1
#define HASHMAP_KEY_FOUND 2

extern const uint64_t EMPTY_INDEX;

typedef struct hashmap_bucket
{
    hash_t hash;
    bool in_use;
    int64_t value;
    char* key;
} hashmap_bucket_t;

typedef struct hashmap_map
{
    uint64_t collisions; // Index lookup collisions count.
    uint64_t rehashes; // Rehash count.
    uint64_t size;
    uint64_t capacity;
    hash_t (* hashf)(const char*);
    hashmap_bucket_t* buckets;
    uint64_t* indices;

#ifdef DEBUG
    // Assertion flag for detecting cascading/recursive rehashing.
    bool debug_no_cascading_rehash;
#endif

} hashmap_map_t;

// Free memory allocated for buckets.
void
hashmap_free_buckets(hashmap_bucket_t* buckets, uint64_t capacity);

// Allocate memory for buckets.
hashmap_bucket_t*
hashmap_init_buckets(uint64_t capacity);

// Initialize map with specific capacity.
hashmap_map_t*
hashmap_init_cap(hash_t (* hashf)(const char*), uint64_t capacity);

// Initialize map with default capacity.
hashmap_map_t*
hashmap_init(hash_t (* hashf)(const char* buffer));

// Free all memory allocated for map.
void
hashmap_free(hashmap_map_t* map);

// Find index in internal bucket array.
// Return code states whether an empty bucket was found
// or if key was already in map.
int64_t
hashmap_lookup_index(hashmap_map_t* map, hash_t hash, char* key, uint64_t* out);

// Add key in map.
int64_t
hashmap_add(hashmap_map_t* map, char* key, int64_t value);

// Add key in map with known hash.
int64_t
hashmap_add_knownhash(hashmap_map_t* map, char* key, int64_t value, hash_t hash);

// Get value from map with key.
int64_t
hashmap_get(hashmap_map_t* map, char* key, int64_t* out);

// Update value behind key in map.
int64_t
hashmap_update(hashmap_map_t* map, char* key, int64_t new_value);

// Increase map size to new capacity.
int64_t
hashmap_rehash(hashmap_map_t* map, uint64_t new_capacity);

// Swap buckets.
void
hashmap_bucket_swap(hashmap_bucket_t* b1, hashmap_bucket_t* b2);

// Sort map by value in ascending order.
// Quicksort with random pivoting.
// Memory for 'out' parameter is allocated in the function.
int64_t
hashmap_sort_by_value(const hashmap_map_t* map, uint64_t low,
                      uint64_t high, hashmap_bucket_t** out);

// Print map contents to stdout.
void
hashmap_print(const hashmap_map_t* map);

#endif //MAPWORDS_HASHMAP_H
