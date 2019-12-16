#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "hashmap.h"

// Size must always be a power of 2 because optimization,
// where modulo is replaced with HASH & (CAPACITY - 1) is used.
#define HASHMAP_INITIAL_CAPACITY 16
#define RESIZE_FACTOR 0.75
#define PRIME 7

hash_t
hashmap_hash1(const hashmap_map_t* const map, hashmap_key_t key)
{
    return map->hashf1(key.key, key.size) & (map->capacity - 1);
}

hash_t
hashmap_hash2(const hashmap_map_t* const map, hashmap_key_t key)
{
    return PRIME - (map->hashf2(key.key, key.size) % PRIME);
}

hashmap_map_t*
hashmap_init_cap(
    hash_t (* hashf1)(const char*, size_t),
    hash_t (* hashf2)(const char*, size_t),
    uint64_t capacity)
{
    if (hashf1 == NULL)
    {
        fprintf(stderr, "hashmap_init_cap(): error: hashf1 == NULL\n");
        return NULL;
    }

    if (hashf2 == NULL)
    {
        fprintf(stderr, "hashmap_init_cap(): error: hashf2 == NULL\n");
        return NULL;
    }

    hashmap_map_t* map = malloc(sizeof(hashmap_map_t));
    if (!map)
    {
        fprintf(stderr, "hashmap_init_cap(): error: malloc(): map\n");
        return NULL;
    }

    map->buckets = calloc(capacity, sizeof(hashmap_bucket_t));
    if (!map->buckets)
    {
        map->hashf1 = NULL;
        map->hashf2 = NULL;
        free(map);
        fprintf(stderr, "hashmap_init_cap(): error: calloc(): map->buckets\n");
        return NULL;
    }

    map->size = 0;
    map->capacity = capacity;
    map->hashf1 = hashf1;
    map->hashf2 = hashf2;

    return map;
}

hashmap_map_t*
hashmap_init(
    hash_t (* hashf1)(const char*, size_t),
    hash_t (* hashf2)(const char*, size_t))
{
    return hashmap_init_cap(hashf1, hashf2, HASHMAP_INITIAL_CAPACITY);
}

void
hashmap_free(hashmap_map_t* map)
{
    if (map != NULL)
    {
        map->hashf1 = NULL;
        map->hashf2 = NULL;
        free(map->buckets);
        free(map);
    }
}

int64_t
hashmap_add(hashmap_map_t* map, hashmap_key_t key, int64_t value)
{
    if (((float) map->size / (float) map->capacity) >= RESIZE_FACTOR)
    {
        uint64_t new_capacity = map->capacity * 2;
        printf("hashmap_add(): load factor exceeded, increasing map capacity to: %lu\n",
               new_capacity);
        int64_t status = hashmap_rehash(map, new_capacity);
        if (status != HASHMAP_OK)
        {
            return status;
        }
    }

    uint64_t index = hashmap_hash1(map, key);
    if (map->buckets[index].in_use)
    {
        if (strcmp(map->buckets[index].key.key, key.key) == 0)
        {
            return HASHMAP_KEY_ALREADY_IN_MAP;
        }

        printf("hashmap_add(): collision on index: %lu\n", index);

        uint64_t index2 = hashmap_hash2(map, key);

        uint64_t i = 1;
        while (true)
        {
            uint64_t new_index = (index + (i * index2)) & (map->capacity - 1);

            if (!map->buckets[new_index].in_use)
            {
                if (strcmp(map->buckets[index].key.key, key.key) == 0)
                {
                    return HASHMAP_KEY_ALREADY_IN_MAP;
                }

                map->buckets[new_index].key = key;
                map->buckets[new_index].value = value;
                map->buckets[new_index].in_use = true;
                map->size++;
                return HASHMAP_OK;
            }
            ++i;
        }
    }
    else
    {
        map->buckets[index].key = key;
        map->buckets[index].value = value;
        map->buckets[index].in_use = true;
        map->size++;
        return HASHMAP_OK;
    }
}

int64_t
hashmap_get(const hashmap_map_t* map, hashmap_key_t key, int64_t* out)
{
    uint64_t index = hashmap_hash1(map, key);
    hashmap_bucket_t bucket;
    if (map->buckets[index].in_use)
    {
        bucket = map->buckets[index];
        if (strcmp(key.key, bucket.key.key) == 0)
        {
            *out = bucket.value;
        }
    }

    uint64_t index2 = hashmap_hash2(map, key);
    for (uint64_t i = 0; i < map->capacity; ++i)
    {
        uint64_t new_index = (index + (i * index2)) & (map->capacity - 1);

        if (!map->buckets[new_index].in_use)
        {
            if (strcmp(key.key, bucket.key.key) == 0)
            {
                *out = bucket.value;
            }
        }
    }

    return HASHMAP_KEY_NOT_FOUND;
}

int64_t
hashmap_remove(hashmap_map_t* map, hashmap_key_t key)
{
    uint64_t index = hashmap_hash1(map, key);
    hashmap_bucket_t* bucket = &map->buckets[index];
    if (bucket->in_use)
    {
        if (strcmp(key.key, bucket->key.key) == 0)
        {
            bucket->in_use = false;
            return HASHMAP_OK;
        }
        uint64_t index2 = hashmap_hash2(map, key);
        for (uint64_t i = 1; i < map->capacity; ++i)
        {
            uint64_t new_index = (index + (i * index2)) & (map->capacity - 1);
            bucket = &map->buckets[new_index];
            if (bucket->in_use)
            {
                if (strcmp(key.key, bucket->key.key) == 0)
                {
                    bucket->in_use = false;
                    return HASHMAP_OK;
                }
            }
        }
    }

    return HASHMAP_KEY_NOT_FOUND;
}

int64_t
hashmap_rehash(hashmap_map_t* map, uint64_t new_capacity)
{
    hashmap_map_t* new_map = hashmap_init_cap(map->hashf1, map->hashf2, new_capacity);
    if (!new_map)
    {
        return HASHMAP_ERROR;
    }

    for (uint64_t i = 0; i < map->capacity; ++i)
    {
        hashmap_bucket_t bucket = map->buckets[i];
        if (bucket.in_use)
        {
            int64_t status = hashmap_add(new_map, bucket.key, bucket.value);
            if (status != HASHMAP_OK && status != HASHMAP_KEY_ALREADY_IN_MAP)
            {
                hashmap_free(new_map);
                return status;
            }
        }
    }

    hashmap_free(map);
    *map = *new_map;
    return HASHMAP_OK;
}
