#include <check.h>
#include <stdlib.h>
#include <stdio.h>

#include "hash.h"
#include "hashmap.h"

hashmap_t* map;

void hashmap_setup(void)
{
    map = hashmap_init(hash_djb2, hash_sdbm);
}

void hashmap_teardown(void)
{
    hashmap_free(map);
}

START_TEST(test_hash_djb2)
{
    hash_t h1 = hash_djb2("test1", 6);
    hash_t h2 = hash_djb2("test2", 6);
    ck_assert_uint_ne(h1, h2);
}
END_TEST

START_TEST(test_hash_sdbm)
{
    hash_t h1 = hash_sdbm("test1", 6);
    hash_t h2 = hash_sdbm("test2", 6);
    ck_assert_uint_ne(h1, h2);
}
END_TEST

START_TEST(test_hash_crc32)
{
    hash_t h1 = hash_sse42_crc32("test1", 6);
    hash_t h2 = hash_sse42_crc32("test2", 6);
    ck_assert_uint_ne(h1, h2);

    hash_t h3 = hash_default_crc32("test1", 6);
    hash_t h4 = hash_default_crc32("test2", 6);
    ck_assert_uint_ne(h3, h4);

    ck_assert_uint_eq(h1, h3);
    ck_assert_uint_eq(h2, h4);
}
END_TEST

START_TEST(test_hashmap_init)
{
    ck_assert_ptr_ne(map, NULL);
    ck_assert_ptr_ne(map->buckets, NULL);
    ck_assert_uint_ne(map->capacity, 0);
}
END_TEST

START_TEST(test_hashmap_add)
{
    char* keystr = "koira";
    hashmap_key_t key = {strlen(keystr) + 1, keystr};
    int value = 666;
    ck_assert_int_ne(HASHMAP_KEY_NOT_FOUND, hashmap_add(map, &key, &value));
}
END_TEST

START_TEST(test_hashmap_find)
{
    char* keystr = "koira";
    hashmap_key_t key = {strlen(keystr) + 1, keystr};
    int value = 666;
    ck_assert_int_eq(HASHMAP_KEY_NOT_FOUND, hashmap_find(map, &key));

    hashmap_add(map, &key, &value);
    ck_assert_int_ne(HASHMAP_KEY_NOT_FOUND, hashmap_find(map, &key));
}
END_TEST

START_TEST(test_hashmap_remove)
{
    char* keystr = "koira";
    hashmap_key_t key = {strlen(keystr) + 1, keystr};
    int value = 666;
    ck_assert_int_eq(HASHMAP_KEY_NOT_FOUND, hashmap_find(map, &key));

    hashmap_add(map, &key, &value);
    ck_assert_int_ne(HASHMAP_KEY_NOT_FOUND, hashmap_find(map, &key));

    hashmap_remove(map, &key);
    ck_assert_int_eq(HASHMAP_KEY_NOT_FOUND, hashmap_find(map, &key));
}
END_TEST

Suite* make_hash_suite(void)
{
    Suite* s;
    TCase* tc_basic;

    s = suite_create("hash");

    tc_basic = tcase_create("basic");

    tcase_add_test(tc_basic, test_hash_djb2);
    tcase_add_test(tc_basic, test_hash_sdbm);
    tcase_add_test(tc_basic, test_hash_crc32);

    suite_add_tcase(s, tc_basic);

    return s;
}

Suite* make_hashmap_suite(void)
{
    Suite* s;
    TCase* tc_basic;

    s = suite_create("hashmap");
    tc_basic = tcase_create("basic");
    tcase_add_checked_fixture(tc_basic, hashmap_setup, hashmap_teardown);

    tcase_add_test(tc_basic, test_hashmap_init);
    tcase_add_test(tc_basic, test_hashmap_add);
    tcase_add_test(tc_basic, test_hashmap_find);
    tcase_add_test(tc_basic, test_hashmap_remove);

    suite_add_tcase(s, tc_basic);

    return s;
}

int main(void)
{
    int number_failed = 0;
    SRunner* sr;

    sr = srunner_create(make_hash_suite());
    srunner_add_suite(sr, make_hashmap_suite());

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
