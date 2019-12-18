#ifndef MAPWORDS_HASHMAP_H
#define MAPWORDS_HASHMAP_H

#include <stddef.h>
#include <stdbool.h>

#include "hash.h"

#define HASHMAP_INITIAL_CAPACITY 16U

#define HASHMAP_OK 0
#define HASHMAP_ERROR -1
#define HASHMAP_KEY_NOT_FOUND 1
#define HASHMAP_KEY_FOUND 2

typedef struct hashmap_bucket
{
    hash_t hash;
    bool in_use;
    int64_t value;
    char* key;
} hashmap_bucket_t;

typedef struct hashmap_map
{
    uint64_t collisions;
    uint64_t size;
    uint64_t capacity;
    hash_t (* hashf)(const char*);
    hashmap_bucket_t* buckets;

#ifdef DEBUG
    bool debug_no_cascading_rehash;
#endif

} hashmap_map_t;

typedef struct hashmap_key_value
{
    uint64_t value;
    char* key;
} hashmap_key_value_t;

hashmap_map_t*
hashmap_init_cap(
    hash_t (* hashf)(const char*),
    uint64_t capacity);

hashmap_map_t*
hashmap_init(
    hash_t (* hashf)(const char* buffer));

void
hashmap_free(hashmap_map_t* map);

int64_t
hashmap_lookup_index(hashmap_map_t* map, hash_t hash, char* key, uint64_t* out);

int64_t
hashmap_add(hashmap_map_t* map, char* key, int64_t value);

int64_t
hashmap_get(hashmap_map_t* map, char* key, int64_t* out);

int64_t
hashmap_update(hashmap_map_t* map, char* key, int64_t new_value);

int64_t
hashmap_rehash(hashmap_map_t* map, uint64_t new_capacity);

int64_t
hashmap_qsort_by_value(const hashmap_map_t* map, hashmap_key_value_t* out);

void
hashmap_print(const hashmap_map_t* map);

#endif //MAPWORDS_HASHMAP_H
