#include <check.h>
#include <stdlib.h>

#include "hash.h"
#include "hashmap.h"

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

Suite* hash_suite()
{
    Suite* s;
    TCase* tc_basic;

    s = suite_create("hash");
    tc_basic = tcase_create("basic");

    tcase_add_test(tc_basic, test_hash_djb2);
    suite_add_tcase(s, tc_basic);

    tcase_add_test(tc_basic, test_hash_sdbm);
    suite_add_tcase(s, tc_basic);

    tcase_add_test(tc_basic, test_hash_crc32);
    suite_add_tcase(s, tc_basic);

    return s;
}

int main()
{
    int number_failed;
    Suite* suite;
    SRunner* srunner;

    suite = hash_suite();
    srunner = srunner_create(suite);

    srunner_run_all(srunner, CK_NORMAL);
    number_failed = srunner_ntests_failed(srunner);
    //srunner_free(srunner); causes free(): invalid pointer with "make test" ???

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
