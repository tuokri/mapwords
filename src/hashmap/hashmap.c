#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "hashmap.h"

#define HASHMAP_INITIAL_CAPACITY 16
#define RESIZE_FACTOR 0.75

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

hashmap_bucket_t*
hashmap_init_buckets(uint64_t capacity)
{
    hashmap_bucket_t* buckets = calloc(capacity, sizeof(hashmap_bucket_t));
    if (!buckets)
    {
        fprintf(stderr, "hashmap_init_buckets(): error: calloc(): map->buckets\n");
        return NULL;
    }

    for (uint64_t i = 0; i < capacity; ++i)
    {
        buckets[i].key = calloc(1, sizeof(char));
        buckets->in_use = false;
        if (!buckets[i].key)
        {
            fprintf(stderr, "hashmap_init_buckets(): error: calloc(): map->buckets.key[%lu]\n", i);
            hashmap_free_buckets(buckets, capacity);
            return NULL;
        }
    }

    return buckets;
}

hashmap_map_t*
hashmap_init_cap(
    hash_t (* hashf)(const char*),
    uint64_t capacity)
{
    if (hashf == NULL)
    {
        fprintf(stderr, "hashmap_init_cap(): error: hashf == NULL\n");
        return NULL;
    }

    hashmap_map_t* map = calloc(1, sizeof(hashmap_map_t));
    if (!map)
    {
        fprintf(stderr, "hashmap_init_cap(): error: calloc(): map\n");
        return NULL;
    }

    map->buckets = hashmap_init_buckets(capacity);
    if(!map->buckets)
    {
        free(map);
        fprintf(stderr, "hashmap_init_cap(): error: hashmap_init_buckets()\n");
        return NULL;
    }

    map->size = 0;
    map->capacity = capacity;
    map->hashf = hashf;

    return map;
}

hashmap_map_t*
hashmap_init(
    hash_t (* hashf)(const char*))
{
    return hashmap_init_cap(hashf, HASHMAP_INITIAL_CAPACITY);
}

void
hashmap_free(hashmap_map_t* map)
{
    if (map != NULL)
    {
        map->hashf = NULL;
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

    if(hashmap_has_key(map, key))
    {
        return HASHMAP_KEY_ALREADY_IN_MAP;
    }

    uint64_t index = map->hashf(key) & (map->capacity - 1);
    if (map->buckets[index].in_use)
    {
        // printf("hashmap_add(): collision on index: %lu\n", index);
        map->collisions++;

        for (uint64_t i = 0; i < map->capacity; ++i)
        {
            uint64_t new_index = (index + i) & (map->capacity - 1);
            if (map->buckets[new_index].in_use)
            {
                map->collisions++;
            }
            break;
        }
    }

    hashmap_bucket_t* bucket = &map->buckets[index];
    if (bucket->key)
    {
        free(bucket->key);
    }
    bucket->key = calloc(strlen(key) + 1, sizeof(char));
    if (!bucket->key)
    {
        return HASHMAP_ERROR;
    }
    strcpy(bucket->key, key);
    bucket->value = value;
    bucket->in_use = true;
    map->size++;
    return HASHMAP_OK;
}

int64_t
hashmap_get(const hashmap_map_t* map, char* key, int64_t* out)
{
    uint64_t index = map->hashf(key) & (map->capacity - 1);

    hashmap_bucket_t bucket = map->buckets[index];
    if (bucket.in_use)
    {
        if (strcmp(key, bucket.key) == 0)
        {
            *out = bucket.value;
            return HASHMAP_OK;
        }
    }

    for (uint64_t i = 0; i < map->capacity; ++i)
    {
        uint64_t new_index = (index + i) & (map->capacity - 1);

        if (map->buckets[new_index].in_use)
        {
            if (strcmp(key, bucket.key) == 0)
            {
                *out = bucket.value;
                return HASHMAP_OK;
            }
        }
    }

    return HASHMAP_KEY_NOT_FOUND;
}

bool
hashmap_has_key(const hashmap_map_t* map, char* key)
{
    uint64_t index = map->hashf(key) & (map->capacity - 1);

    hashmap_bucket_t bucket = map->buckets[index];
    if (bucket.in_use)
    {
        if (strcmp(key, bucket.key) == 0)
        {
            return true;
        }
    }

    for (uint64_t i = 0; i < map->capacity; ++i)
    {
        uint64_t new_index = (index + i) & (map->capacity - 1);

        if (map->buckets[new_index].in_use)
        {
            if (strcmp(key, bucket.key) == 0)
            {
                return true;
            }
        }
    }

    return false;
}

int64_t
hashmap_rehash(hashmap_map_t* map, uint64_t new_capacity)
{
    hashmap_bucket_t* new_buckets = hashmap_init_buckets(new_capacity);
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

//                for (uint64_t x = 0; x < old_capacity; ++x)
//                {
//                    hashmap_bucket_t bucket = old_buckets[x];
//                    if (bucket.in_use)
//                    {
//                        printf("old_map (%lu): [%s->%lu]\n", x, bucket.key, bucket.value);
//                    }
//                }
//
//                for (uint64_t z = 0; z < new_capacity; ++z)
//                {
//                    hashmap_bucket_t bucket = new_buckets[z];
//                    if (bucket.in_use)
//                    {
//                        printf("new_map (%lu): [%s->%lu]\n", z, bucket.key, bucket.value);
//                    }
//                }

                hashmap_free_buckets(old_buckets, old_capacity);
                return status;
            }
        }
    }

    hashmap_free_buckets(old_buckets, old_capacity);
    return HASHMAP_OK;
}
