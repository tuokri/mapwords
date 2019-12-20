#include "hash.h"
#include "hashmap.h"
#include "greatest.h"
#include "common.h"

static hashmap_map_t* MAP;

uint64_t count_used_buckets(hashmap_bucket_t* buckets, uint64_t capacity)
{
    uint64_t count = 0;
    for (uint64_t i = 0; i < capacity; ++i)
    {
        if (buckets[i].in_use)
        {
            ++count;
        }
    }
    return count;
}

uint64_t count_used_buckets_in_map(hashmap_map_t* map)
{
    return count_used_buckets(map->buckets, map->capacity);
}

TEST rehash_grow(void)
{
    ASSERT_EQ(HASHMAP_INITIAL_CAPACITY, MAP->capacity);
    ASSERT_EQ(0, MAP->size);

    ASSERT_EQ(HASHMAP_OK, hashmap_add(MAP, "test_key1", 420));
    ASSERT_EQ(HASHMAP_OK, hashmap_add(MAP, "test_key2", 421));
    ASSERT_EQ(HASHMAP_OK, hashmap_add(MAP, "test_key3", 422));

    hashmap_rehash(MAP, MAP->capacity * 2);

    int64_t out;
    ASSERT_EQ(HASHMAP_KEY_FOUND, hashmap_get(MAP, "test_key1", &out));
    ASSERT_EQ(420, out);

    ASSERT_EQ(HASHMAP_KEY_FOUND, hashmap_get(MAP, "test_key2", &out));
    ASSERT_EQ(421, out);

    ASSERT_EQ(HASHMAP_KEY_FOUND, hashmap_get(MAP, "test_key3", &out));
    ASSERT_EQ(422, out);

    ASSERT_EQ(HASHMAP_INITIAL_CAPACITY * 2, MAP->capacity);
    ASSERT_EQ(3, MAP->size);

    // Adding same keys after rehash should not mutate MAP.
    ASSERT_EQ(HASHMAP_KEY_FOUND, hashmap_add(MAP, "test_key1", 555));
    ASSERT_EQ(HASHMAP_KEY_FOUND, hashmap_add(MAP, "test_key2", 444));
    ASSERT_EQ(HASHMAP_KEY_FOUND, hashmap_add(MAP, "test_key3", 333));
    ASSERT_EQ(3, MAP->size);

    PASS();
}

TEST rehash_shrink(void)
{
    ASSERT_EQ(HASHMAP_ERROR, hashmap_rehash(MAP, MAP->capacity / 2));
    PASS();
}

TEST duplicate_add(void)
{
    int64_t status;
    int64_t out = -1;

    ASSERT_EQ(0, MAP->size);
    status = hashmap_get(MAP, "test_key1", &out);
    ASSERT_EQ(HASHMAP_KEY_NOT_FOUND, status);
    ASSERT_EQ(-1, out);

    status = hashmap_add(MAP, "test_key1", 420);
    ASSERT_EQ(HASHMAP_OK, status);
    ASSERT_EQ(1, MAP->size);

    status = hashmap_add(MAP, "test_key1", 555);
    ASSERT_EQ(HASHMAP_KEY_FOUND, status);
    ASSERT_EQ(1, MAP->size);

    status = hashmap_add(MAP, "test_key1", 666);
    ASSERT_EQ(HASHMAP_KEY_FOUND, status);
    ASSERT_EQ(1, MAP->size);

    PASS();
}

TEST load_factor(void)
{
    char msg[512] = {'\0'};
    uint64_t bucket_count = 0;

    // 13 chars -> trigger rehash with RESIZE_FACTOR == 0.75
    // and HASHMAP_INITIAL_CAPACITY == 16. New capacity is 32.
    const char test_str_1[14] = "abcdefghijklm";

    // Trigger second rehash with 12 more chars. New capacity is 64.
    const char test_str_2[13] = "nopqrstuvwxy";

    int64_t status;
    for (uint64_t i = 0; i < strlen(test_str_1); ++i)
    {
        char c[2] = {test_str_1[i]};
        status = hashmap_add(MAP, c, (int) c[0]);
        sprintf(msg, "error on hashmap_add(): c=%s, status=%ld", c, status);
        ASSERT_EQm(msg, HASHMAP_OK, status);
    }

    bucket_count = count_used_buckets_in_map(MAP);
    sprintf(msg, "bucket_count=%lu", bucket_count);
    ASSERT_EQm(msg, 13, bucket_count);
    ASSERT_EQ(HASHMAP_INITIAL_CAPACITY * 2, MAP->capacity);
    ASSERT_EQ(strlen(test_str_1), MAP->size);

    for (uint64_t i = 0; i < strlen(test_str_2); ++i)
    {
        char c[2] = {test_str_2[i]};
        status = hashmap_add(MAP, c, (int) c[0]);
        sprintf(msg, "error on hashmap_add(): c=%s, status=%ld", c, status);
        ASSERT_EQm(msg, HASHMAP_OK, status);
    }

    // "y\0" triggered rehash above. Check edge case.
    ASSERT_EQ(HASHMAP_KEY_FOUND, hashmap_add(MAP, "y", (int) 'y'));

    bucket_count = count_used_buckets_in_map(MAP);
    sprintf(msg, "bucket_count=%lu", bucket_count);
    ASSERT_EQm(msg, 25, bucket_count);
    ASSERT_EQ(HASHMAP_INITIAL_CAPACITY * 2 * 2, MAP->capacity);
    ASSERT_EQ(strlen(test_str_1) + strlen(test_str_2), MAP->size);

    char test_str_3[32] = {'\0'};
    strcat(test_str_3, test_str_1);
    strcat(test_str_3, test_str_2);

    ASSERT_EQ(strlen(test_str_3), strlen(test_str_1) + strlen(test_str_2));

    // Add new keys until rehash triggered. New capacity is 128.
    uint64_t more = 24;
    char new_key[256] = {'\0'};
    for(uint64_t i = 0; i < more; ++i)
    {
        sprintf(new_key, "blob%lu", i);
        ASSERT_EQ(HASHMAP_OK, hashmap_add(MAP, new_key, i));
    }
    ASSERT_EQ(strlen(test_str_3) + more, MAP->size);

    uint64_t correct_sz = strlen(test_str_3) + more;
    // Add some old keys again. Should not mutate map.
    for (uint64_t i = 0; i < strlen(test_str_3); ++i)
    {
        char c[2] = {test_str_3[i]};
        status = hashmap_add(MAP, c, (int) c[0]);

        sprintf(msg, "error on hashmap_add(): c=%s, size changed to: %lu from: %lu",
                c, MAP->size, correct_sz);
        ASSERT_EQm(msg, correct_sz, MAP->size);

        sprintf(msg, "error on hashmap_add(): c=%s, status=%ld", c, status);
        ASSERT_EQm(msg, HASHMAP_KEY_FOUND, status);
    }

    bucket_count = count_used_buckets_in_map(MAP);
    sprintf(msg, "bucket_count=%lu", bucket_count);
    ASSERT_EQm(msg, 25 + more, bucket_count);
    sprintf(msg, "MAP->capacity=%lu != %u", MAP->capacity,
            HASHMAP_INITIAL_CAPACITY * 2 * 2 * 2);
    ASSERT_EQm(msg, HASHMAP_INITIAL_CAPACITY * 2 * 2 * 2, MAP->capacity);
    ASSERT_EQ(correct_sz, MAP->size);

    PASS();
}

TEST update(void)
{
    int64_t status;
    int64_t out = -1;
    char msg[512] = {'\0'};

    status = hashmap_add(MAP, "test_key69", 420);
    ASSERT_EQ(HASHMAP_OK, status);

    status = hashmap_update(MAP, "test_key69", 666);
    ASSERT_EQ(HASHMAP_OK, status);

    status = hashmap_get(MAP, "test_key69", &out);
    sprintf(msg, "error on hashmap_get(): status=%ld", status);
    ASSERT_EQm(msg, HASHMAP_KEY_FOUND, status);
    ASSERT_EQ(666, out);

    PASS();
}

TEST sort(void)
{
    uint64_t more = 32;
    char new_key[256] = {'\0'};
    for(uint64_t i = 0; i < more; ++i)
    {
        sprintf(new_key, "blob%lu", i);
        ASSERT_EQ(HASHMAP_OK, hashmap_add(MAP, new_key, i));
    }

    ASSERT_EQ(HASHMAP_OK, hashmap_add(MAP, "blob555", 55));
    ASSERT_EQ(HASHMAP_OK, hashmap_add(MAP, "blob666", 55));

    hashmap_bucket_t* sorted = NULL;
    ASSERT_EQ(HASHMAP_OK, hashmap_sort_by_value(
        MAP, 0, MAP->capacity - 1, &sorted));

    ASSERT_EQ(count_used_buckets(sorted, MAP->capacity),
        count_used_buckets_in_map(MAP));

    for(uint64_t i = 0; i < MAP->capacity; ++i)
    {
        printf("[%lu]: %s->%lu (in_use=%d, hash=%lu)\n",
            i, sorted[i].key, sorted[i].value, sorted[i].in_use, sorted[i].hash);
        ASSERT_FALSE(sorted[i].key == NULL);
        ASSERT(sorted[i].in_use == false || sorted[i].in_use == true);
    }

    hashmap_free_buckets(sorted, MAP->capacity);
    PASS();
}

TEST swap(void)
{
    uint64_t more = 32;
    char new_key[256] = {'\0'};
    for(uint64_t i = 0; i < more; ++i)
    {
        sprintf(new_key, "blob%lu", i);
        ASSERT_EQ(HASHMAP_OK, hashmap_add(MAP, new_key, i));
    }

    hashmap_bucket_t* b1;
    hashmap_bucket_t* b2;

    b1 = &(MAP->buckets)[52];
    b2 = &(MAP->buckets)[56];
    uint64_t original_52 = b1->value;
    uint64_t original_56 = b2->value;

    hashmap_bucket_swap(b1, b2);

    ASSERT(original_52 == MAP->buckets[56].value);
    ASSERT(original_56 == MAP->buckets[52].value);

    PASS();
}

SUITE (hashmap_suite)
{
    MAP = hashmap_init(hash_djb2);
    RUN_TEST(rehash_grow);
    hashmap_free(MAP);

    MAP = hashmap_init(hash_djb2);
    RUN_TEST(rehash_shrink);
    hashmap_free(MAP);

    MAP = hashmap_init(hash_djb2);
    RUN_TEST(duplicate_add);
    hashmap_free(MAP);

    MAP = hashmap_init(hash_djb2);
    RUN_TEST(duplicate_add);
    hashmap_free(MAP);

    MAP = hashmap_init(hash_djb2);
    RUN_TEST(load_factor);
    hashmap_free(MAP);

    MAP = hashmap_init(hash_djb2);
    RUN_TEST(update);
    hashmap_free(MAP);

    MAP = hashmap_init(hash_djb2);
    RUN_TEST(sort);
    hashmap_free(MAP);

    MAP = hashmap_init(hash_djb2);
    RUN_TEST(swap);
    hashmap_free(MAP);
}

GREATEST_MAIN_DEFS();

int main(int argc, char** argv)
{
    GREATEST_MAIN_BEGIN();

    RUN_SUITE(hashmap_suite);

    GREATEST_MAIN_END();
}
