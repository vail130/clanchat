#include <check.h>

#include "../src/clanchat.h"

START_TEST(test_clanchat)
{

}
END_TEST

Suite * filesearch_suite (void) {
    Suite *s;
    TCase *tc_core;

    s = suite_create("Filesearch");
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_clanchat);
    suite_add_tcase(s, tc_core);

    return s;
}

int main (void) {
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = filesearch_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
