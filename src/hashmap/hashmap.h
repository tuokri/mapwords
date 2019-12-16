#ifndef MAPWORDS_HASHMAP_H
#define MAPWORDS_HASHMAP_H

#include <stddef.h>
#include <stdbool.h>

#include "hash.h"

#define HASHMAP_OK 0
#define HASHMAP_ERROR -1
#define HASHMAP_KEY_NOT_FOUND 1
#define HASHMAP_KEY_ALREADY_IN_MAP 2

typedef struct hashmap_bucket
{
    bool in_use;
    int64_t value;
    char* key;
} hashmap_bucket_t;

typedef struct hashmap_map
{
    uint64_t size;
    uint64_t capacity;
    hash_t (* hashf1)(const char*);
    hash_t (* hashf2)(const char*);
    hashmap_bucket_t* buckets;
} hashmap_map_t;

hashmap_map_t*
hashmap_init_cap(
    hash_t (* hashf1)(const char*),
    hash_t (* hashf2)(const char*),
    uint64_t capacity);

hashmap_map_t*
hashmap_init(
    hash_t (* hashf1)(const char* buffer),
    hash_t (* hashf2)(const char* buffer));

void
hashmap_free(hashmap_map_t* map);

int64_t
hashmap_add(hashmap_map_t* map, char* key, int64_t value);

int64_t
hashmap_get(const hashmap_map_t* map, char* key, int64_t* out);

int64_t
hashmap_remove(hashmap_map_t* map, char* key);

int64_t
hashmap_rehash(hashmap_map_t* map, uint64_t new_capacity);

#endif //MAPWORDS_HASHMAP_H
