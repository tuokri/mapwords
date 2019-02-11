#include <check.h>
#include <stdlib.h>

#include "hash.h"
#include "hashmap.h"

START_TEST(test_hash_djb2)
{
    hash_t h1 = hash_djb2("test1", 6);
    hash_t h2 = hash_djb2("test2", 6);
    fail_if(h1 == h2);
}
END_TEST

Suite* hash_suite()
{
    Suite* s;
    TCase* tc_core;

    s = suite_create("hash");
    tc_core = tcase_create("core");

    tcase_add_test(tc_core, test_hash_djb2);
    suite_add_tcase(s, tc_core);

    return s;
}

int main()
{
    int number_failed;
    Suite* s;
    SRunner* sr;

    s = hash_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
