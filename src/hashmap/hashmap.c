#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/random.h>

#ifdef DEBUG

#include <assert.h>

#endif

#include "hash.h"
#include "hashmap.h"

#define RESIZE_FACTOR 0.75
#define PERTURB_SHIFT 5U

#ifdef DEBUG
#define IS_POWER_OF_2(x) (((x) & (x - 1)) == 0)
#endif

void
hashmap_free_buckets(hashmap_bucket_t* buckets, uint64_t capacity)
{
    if (!buckets)
    {
        return;
    }

    for (uint64_t i = 0; i < capacity; i++)
    {
        hashmap_bucket_t* bucket = &buckets[i];
        if (bucket->key)
        {
            free(bucket->key);
        }
        bucket->in_use = false;
        bucket->hash = 0;
        bucket->value = 0;
    }
    free(buckets);
}

hashmap_bucket_t*
hashmap_init_buckets(uint64_t capacity)
{
    hashmap_bucket_t* buckets = calloc(capacity, sizeof(hashmap_bucket_t));
    if (!buckets)
    {
        fprintf(stderr, "hashmap_init_buckets(): error: calloc(): "
                        "map->buckets\n");
        return NULL;
    }

    for (uint64_t i = 0; i < capacity; ++i)
    {
        buckets[i].key = calloc(1, sizeof(char));
        if (!buckets[i].key)
        {
            fprintf(stderr, "hashmap_init_buckets(): error: calloc(): "
                            "map->buckets.key[%lu]\n", i);
            hashmap_free_buckets(buckets, capacity);
            return NULL;
        }
        *buckets[i].key = '\0';
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
    if (!map->buckets)
    {
        free(map);
        fprintf(stderr, "hashmap_init_cap(): error: hashmap_init_buckets()\n");
        return NULL;
    }

    map->size = 0;
    map->capacity = capacity;
    map->hashf = hashf;

    void* buf = malloc(sizeof(int) * 32);
    getrandom(buf, 32, GRND_RANDOM);
    unsigned seed = *(unsigned*) buf;
    srand(seed);
    free(buf);

    return map;
}

hashmap_map_t*
hashmap_init(hash_t (* hashf)(const char*))
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
hashmap_lookup_index(hashmap_map_t* map, hash_t hash, char* key, uint64_t* out)
{
    uint64_t index = hash & (map->capacity - 1);
    hashmap_bucket_t* bucket = &map->buckets[index];

    if (bucket->in_use)
    {
        if ((strcmp(bucket->key, key) == 0) && (bucket->hash == hash))
        {
            *out = index;
            return HASHMAP_KEY_FOUND;
        }

        // printf("hashmap_add(): collision on index: %lu\n", index);
        map->collisions++;
        uint64_t perturb = hash;

        while (true)
        {
            perturb >>= PERTURB_SHIFT;
            index = (index * 5 + perturb + 1) & (map->capacity - 1);
            bucket = &map->buckets[index];

            if (bucket->in_use)
            {
                if ((strcmp(bucket->key, key) == 0) && (bucket->hash == hash))
                {
                    *out = index;
                    return HASHMAP_KEY_FOUND;
                }
            } else
            {
                break;
            }
        }
    }

    // Empty bucket found for this key.
    *out = index;
    return HASHMAP_KEY_NOT_FOUND;
}

int64_t
hashmap_add(hashmap_map_t* map, char* key, int64_t value)
{
    hash_t hash = map->hashf(key);
    return hashmap_add_knownhash(map, key, value, hash);
}

int64_t
hashmap_add_knownhash(hashmap_map_t* map, char* key, int64_t value, hash_t hash)
{
    uint64_t index = 0;
    int64_t status = hashmap_lookup_index(map, hash, key, &index);
    if (status == HASHMAP_KEY_FOUND)
    {
        return status;
    }

    if ((((float) map->size) / (float) map->capacity) >= RESIZE_FACTOR)
    {
#ifdef DEBUG
        printf("hashmap_add(): load factor: %f >= %f\n",
               ((float) map->size) / (float) map->capacity, RESIZE_FACTOR);
        assert(!map->debug_no_cascading_rehash);
#endif

        uint64_t new_capacity = map->capacity * 2;

#ifdef DEBUG
        printf("hashmap_add(): load factor exceeded with size: %lu, "
               "increasing map capacity to: %lu\n",
               map->size, new_capacity);
#endif

        status = hashmap_rehash(map, new_capacity);

        if (status != HASHMAP_OK)
        {
            fprintf(stderr, "hashmap_add(): error: hashmap_rehash() "
                            "status=%lu\n", status);
            return status;
        }

        // Need to find index again after rehash.
        status = hashmap_lookup_index(map, hash, key, &index);
        if (status != HASHMAP_KEY_NOT_FOUND)
        {
            fprintf(stderr, "hashmap_add(): error: hashmap_lookup_index() "
                            "status=%lu\n", status);
            return status;
        }
    }

    hashmap_bucket_t* bucket = &map->buckets[index];

    bucket->key = realloc(bucket->key, sizeof(char) * strlen(key) + 1);
    if (!bucket->key)
    {
        return HASHMAP_ERROR;
    }

    strcpy(bucket->key, key);
    bucket->value = value;
    bucket->in_use = true;
    bucket->hash = hash;
    map->size++;
    return HASHMAP_OK;
}

int64_t
hashmap_get(hashmap_map_t* map, char* key, int64_t* out)
{
    hash_t hash = map->hashf(key);
    uint64_t index = 0;
    int64_t status = hashmap_lookup_index(map, hash, key, &index);
    if (status == HASHMAP_KEY_FOUND
        )
    {
        *out = map->buckets[index].value;
    }

    return status;
}

int64_t
hashmap_update(hashmap_map_t* map, char* key, int64_t new_value)
{
    hash_t hash = map->hashf(key);
    uint64_t index = 0;
    int64_t status = hashmap_lookup_index(map, hash, key, &index);
    if (status == HASHMAP_KEY_FOUND)
    {
        hashmap_bucket_t* bucket = &map->buckets[index];
        bucket->value = new_value;
        status = HASHMAP_OK;
    }
    return status;
}

int64_t
hashmap_rehash(hashmap_map_t* map, uint64_t new_capacity)
{
    // Shrinking not implemented.
    if (new_capacity < map->capacity)
    {
        return HASHMAP_ERROR;
    }

#ifdef DEBUG
    assert(IS_POWER_OF_2(new_capacity));
    map->debug_no_cascading_rehash = true;
#endif

    hashmap_bucket_t* new_buckets = hashmap_init_buckets(new_capacity);
    if (!new_buckets)
    {
        return HASHMAP_ERROR;
    }

    uint64_t old_capacity = map->capacity;
    hashmap_bucket_t* old_buckets = map->buckets;
    map->buckets = new_buckets;
    map->capacity = new_capacity;
    map->size = 0;

    for (uint64_t i = 0; i < old_capacity; ++i)
    {
        hashmap_bucket_t old_bucket = old_buckets[i];
#ifdef  DEBUG
        if (!old_bucket.in_use)
        {
            assert(strcmp(old_bucket.key, "") == 0);
            assert(old_bucket.value == 0);
            assert(old_bucket.hash == 0);
        }
#endif

        if (old_bucket.in_use)
        {
#ifdef DEBUG
            printf("hashmap_rehash(): rehashing from old bucket: "
                   "%s->%lu (hash=%lu)\n",
                   old_bucket.key, old_bucket.value, old_bucket.hash);
#endif

            int64_t status = hashmap_add_knownhash(
                map, old_bucket.key, old_bucket.value, old_bucket.hash);

            if (status != HASHMAP_OK)
            {
                fprintf(stderr, "hashmap_rehash(): error: hashmap_add() status=%lu on "
                                "key=%s, value=%ld, hash=%lu, old_capacity=%lu, new_capacity=%lu\n",
                        status, old_bucket.key, old_bucket.value, old_bucket.hash,
                        old_capacity, new_capacity);

                hashmap_free_buckets(old_buckets, old_capacity);
                return status;
            }
        }
    }

#ifdef DEBUG
    map->debug_no_cascading_rehash = false;
#endif

    hashmap_free_buckets(old_buckets, old_capacity);
    return HASHMAP_OK;
}

void
hashmap_bucket_swap(hashmap_bucket_t* b1, hashmap_bucket_t* b2)
{
#ifdef DEBUG
    assert(b1 != NULL);
    assert(b2 != NULL);
#endif

    hashmap_bucket_t tempb = *b1;
    *b1 = *b2;
    *b2 = tempb;
}

// Lomuto's partition scheme.
uint64_t
hashmap_buckets_partition(hashmap_bucket_t* buckets,
                          uint64_t low, uint64_t high)
{
    int64_t pivot = buckets[high].value;
    uint64_t i = low;

    for (uint64_t j = low; j <= high - 1; ++j)
    {
        if (buckets[j].value <= pivot)
        {
            hashmap_bucket_swap(&buckets[i], &buckets[j]);
            ++i;
        }
    }

    hashmap_bucket_swap(&buckets[i], &buckets[high]);
    return i;
}

uint64_t
hashmap_buckets_partition_r(hashmap_bucket_t* buckets,
                            uint64_t low, uint64_t high)
{
    uint64_t random = low + rand() % (high - low);

#ifdef DEBUG
    assert(buckets != NULL);
    assert(low >= 0);
    assert(high >= 0);
    assert(random >= low);
    assert(random <= high);
#endif

    hashmap_bucket_swap(&buckets[random], &buckets[high]);
    return hashmap_buckets_partition(buckets, low, high);
}

void
hashmap_sort_by_value_recurse(uint64_t low, uint64_t high, hashmap_bucket_t* out)
{
    if (low < high)
    {
        uint64_t pivot = hashmap_buckets_partition_r(out, low, high);
        hashmap_sort_by_value_recurse(low, pivot - 1, out);
        hashmap_sort_by_value_recurse(pivot + 1, high, out);
    }
}
//
int64_t
hashmap_sort_by_value(const hashmap_map_t* map, uint64_t low,
                      uint64_t high, hashmap_bucket_t** out)
{
    if (*out != NULL)
    {
        fprintf(stderr, "hashmap_sort_by_value(): expected 'out' == NULL\n");
        return HASHMAP_ERROR;
    }

    if (low > high)
    {
        fprintf(stderr, "hashmap_sort_by_value(): low must "
                        "be less than high\n");
        return HASHMAP_ERROR;
    }

    if (low < 0)
    {
        fprintf(stderr, "hashmap_sort_by_value(): low must "
                        "be greater zero\n");
        return HASHMAP_ERROR;
    }

    if (high >= map->capacity)
    {
        fprintf(stderr, "hashmap_sort_by_value(): high must "
                        "be lower than %lu\n", map->capacity);
        return HASHMAP_ERROR;
    }

    uint64_t pivot;
    hashmap_bucket_t* bucket;
    hashmap_bucket_t* out_bucket;

    *out = calloc(map->capacity, sizeof(hashmap_bucket_t));
    if (!*out)
    {
        fprintf(stderr, "hashmap_sort_by_value(): error: calloc(): 'out'\n");
        return HASHMAP_ERROR;
    }

    for (uint64_t i = 0; i < map->capacity; ++i)
    {
        bucket = &map->buckets[i];
        out_bucket = *out;

        out_bucket[i].hash = bucket->hash;
        out_bucket[i].in_use = bucket->in_use;
        out_bucket[i].value = bucket->value;

        out_bucket[i].key = calloc(strlen(bucket->key) + 1, sizeof(char));
        if (!out_bucket[i].key)
        {
            hashmap_free_buckets(*out, map->capacity);
            return HASHMAP_ERROR;
        }

        strcpy(out_bucket[i].key, bucket->key);
    }

    if (low < high)
    {
        pivot = hashmap_buckets_partition_r(*out, low, high);
        hashmap_sort_by_value_recurse(low, pivot - 1, *out);
        hashmap_sort_by_value_recurse(pivot + 1, high, *out);
    }

    return HASHMAP_OK;
}

void
hashmap_print(const hashmap_map_t* const map)
{
    for (uint64_t i = 0; i < map->capacity; ++i)
    {
        printf("[%lu]: %s->%lu (in_use=%d, hash=%lu)\n", i,
               map->buckets[i].key, map->buckets[i].value,
               map->buckets[i].in_use, map->buckets[i].hash);
    }
}
