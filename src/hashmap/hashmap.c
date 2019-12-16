#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "hashmap.h"

#define HASHMAP_INITIAL_CAPACITY 16
#define RESIZE_FACTOR 0.75

hashmap_map_t*
hashmap_init_cap(
    hash_t (* hashf1)(const char*),
    hash_t (* hashf2)(const char*),
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

    hashmap_map_t* map = calloc(1, sizeof(hashmap_map_t));
    if (!map)
    {
        fprintf(stderr, "hashmap_init_cap(): error: calloc(): map\n");
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
    hash_t (* hashf1)(const char*),
    hash_t (* hashf2)(const char*))
{
    return hashmap_init_cap(hashf1, hashf2, HASHMAP_INITIAL_CAPACITY);
}

void
hashmap_free_buckets(hashmap_bucket_t* buckets, uint64_t capacity)
{
    for (uint64_t i = 0; i < capacity; i++)
    {
        hashmap_bucket_t bucket = buckets[i];
        if (bucket.key)
        {
            free(bucket.key);
        }
    }
    free(buckets);
}

void
hashmap_free(hashmap_map_t* map)
{
    if (map != NULL)
    {
        map->hashf1 = NULL;
        map->hashf2 = NULL;
        hashmap_free_buckets(map->buckets, map->capacity);
        free(map);
    }
}

int64_t
hashmap_add(hashmap_map_t* map, char* key, int64_t value)
{
    if (((float) map->size / (float) map->capacity) >= RESIZE_FACTOR)
    {
        uint64_t new_capacity = map->capacity * 2;
        printf("hashmap_add(): load factor exceeded, increasing map capacity to: %lu\n",
               new_capacity);
        int64_t status = hashmap_rehash(map, new_capacity);
        if (status != HASHMAP_OK)
        {
            printf("hashmap_add(): error: hashmap_rehash() status=%lu\n", status);
            return status;
        }
    }

    hash_t hash1 = map->hashf1(key);
    uint64_t index = hash1 & (map->capacity - 1);
    if (map->buckets[index].in_use)
    {
        if (strcmp(map->buckets[index].key, key) == 0)
        {
            return HASHMAP_KEY_ALREADY_IN_MAP;
        }

        // printf("hashmap_add(): collision on index: %lu\n", index);

        hash_t hash_2 = map->hashf2(key);

        uint64_t i = 1;
        while (true)
        {
            uint64_t new_index = (hash1 + (i * hash_2)) & (map->capacity - 1);

            if (map->buckets[new_index].in_use)
            {
                if (strcmp(map->buckets[index].key, key) == 0)
                {
                    return HASHMAP_KEY_ALREADY_IN_MAP;
                }
            }
            else
            {
                break;
            }
            ++i;

            // Fall back to linear probing...
            if (i > map->capacity)
            {
                for (i = 0; i < map->capacity; ++i)
                {
                    if (map->buckets[i].in_use)
                    {
                        if (strcmp(map->buckets[index].key, key) == 0)
                        {
                            return HASHMAP_KEY_ALREADY_IN_MAP;
                        }
                    }
                }
            }
        }
    }

    if (map->buckets[index].key)
    {
        free(map->buckets[index].key);
    }
    map->buckets[index].key = calloc(strlen(key), sizeof(char));
    if (!map->buckets[index].key)
    {
        return HASHMAP_ERROR;
    }
    strcpy(map->buckets[index].key, key);
    map->buckets[index].value = value;
    map->buckets[index].in_use = true;
    map->size++;
    return HASHMAP_OK;
}

int64_t
hashmap_get(const hashmap_map_t* map, char* key, int64_t* out)
{
    hash_t hash1 = map->hashf1(key);
    uint64_t index = hash1 & (map->capacity - 1);

    hashmap_bucket_t bucket;
    if (map->buckets[index].in_use)
    {
        bucket = map->buckets[index];
        if (strcmp(key, bucket.key) == 0)
        {
            *out = bucket.value;
        }
    }

    uint64_t index2 = map->hashf2(key);
    for (uint64_t i = 0; i < map->capacity; ++i)
    {
        uint64_t new_index = (hash1 + (i * index2)) & (map->capacity - 1);

        if (map->buckets[new_index].in_use)
        {
            if (strcmp(key, bucket.key) == 0)
            {
                *out = bucket.value;
            }
        }
    }

    return HASHMAP_KEY_NOT_FOUND;
}

int64_t
hashmap_remove(hashmap_map_t* map, char* key)
{
    hash_t hash1 = map->hashf1(key);
    uint64_t index = hash1 & (map->capacity - 1);

    hashmap_bucket_t* bucket = &map->buckets[index];
    if (bucket->in_use)
    {
        if (strcmp(key, bucket->key) == 0)
        {
            bucket->in_use = false;
            return HASHMAP_OK;
        }
        uint64_t index2 = map->hashf2(key);
        for (uint64_t i = 1; i < map->capacity; ++i)
        {
            uint64_t new_index = (hash1 + (i * index2)) & (map->capacity - 1);
            bucket = &map->buckets[new_index];
            if (bucket->in_use)
            {
                if (strcmp(key, bucket->key) == 0)
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
    hashmap_bucket_t* new_buckets = calloc(new_capacity, sizeof(hashmap_bucket_t));
    if (!new_buckets)
    {
        return HASHMAP_ERROR;
    }

    uint64_t old_capacity = map->capacity;
    hashmap_bucket_t* old_buckets = map->buckets;
    map->buckets = new_buckets;
    map->capacity = new_capacity;

    for (uint64_t i = 0; i < old_capacity; ++i)
    {
        hashmap_bucket_t old_bucket = old_buckets[i];
        if (old_bucket.in_use)
        {
            int64_t status = hashmap_add(map, old_bucket.key, old_bucket.value);
            if (status != HASHMAP_OK)
            {
                printf("hashmap_rehash(): error: hashmap_add() status=%lu on key=%s, value=%ld\n",
                       status, old_bucket.key, old_bucket.value);

                for (uint64_t x = 0; x < old_capacity; ++x)
                {
                    hashmap_bucket_t bucket = old_buckets[x];
                    if (bucket.in_use)
                    {
                        printf("old_map (%lu): [%s->%lu]\n", x, bucket.key, bucket.value);
                    }
                }

                for (uint64_t z = 0; z < new_capacity; ++z)
                {
                    hashmap_bucket_t bucket = new_buckets[z];
                    if (bucket.in_use)
                    {
                        printf("new_map (%lu): [%s->%lu]\n", z, bucket.key, bucket.value);
                    }
                }

                hashmap_free_buckets(old_buckets, old_capacity);
                return status;
            }
        }
    }

    hashmap_free_buckets(old_buckets, old_capacity);
    return HASHMAP_OK;
}
