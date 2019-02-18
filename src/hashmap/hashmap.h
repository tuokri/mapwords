#ifndef MAPWORDS_HASHMAP_H
#define MAPWORDS_HASHMAP_H

#include <stddef.h>
#include <stdbool.h>

#include "hash.h"

#define HASHMAP_KEY_NOT_FOUND -1

typedef enum hashmap_bucket_status
{
    hashmap_bucket_status_free,
    hashmap_bucket_status_used,
    hashmap_bucket_status_deleted
} hashmap_bucket_status_t;

typedef struct hashmap_key
{
    size_t size;
    char* key;
} hashmap_key_t;

typedef struct hashmap_bucket
{
    hashmap_bucket_status_t status;
    hashmap_key_t* key;
    void* value;
} hashmap_bucket_t;

typedef struct hashmap_map
{
    uint32_t lookup_index;
    uint32_t size;
    uint32_t capacity;
    hash_t (*hashf1)(char*, size_t);
    hash_t (*hashf2)(char*, size_t);
    hashmap_bucket_t* buckets;
} hashmap_t;

hash_t
hashmap_doublehash(hashmap_t* map, hashmap_key_t* key,
    uint32_t probe_index);

hashmap_t*
hashmap_init_cap(
    hash_t (*hashf1)(char* buffer, size_t size),
    hash_t (*hashf2)(char* buffer, size_t size),
    uint32_t init_capacity);

hashmap_t*
hashmap_init(
    hash_t (*hashf1)(char* buffer, size_t size),
    hash_t (*hashf2)(char* buffer, size_t size));

void
hashmap_free(hashmap_t* map);

int64_t
hashmap_add(hashmap_t* map, hashmap_key_t* key, void* value);

int64_t
hashmap_find(hashmap_t* map, hashmap_key_t* key);

int64_t
hashmap_remove(hashmap_t* map, hashmap_key_t* key);

bool
hashmap_resize(hashmap_t* map, uint32_t new_capacity);

bool
hashmap_rehash(hashmap_t* map, hashmap_t* new_map);

void
hashmap_clear(hashmap_t* map);

uint32_t
hashmap_lookup_prime_index(hashmap_t* map, const uint32_t* lookup,
    uint32_t target);

#endif //MAPWORDS_HASHMAP_H
