#include <check.h>
#include <stdlib.h>

#include "../src/clanchat.h"

START_TEST(test_error_if_name_empty)
{
    int retval = clanchat("");
    ck_assert_msg (retval == -1, "Empty name should be an error.");
}
END_TEST

Suite * clanchat_suite (void) {
    Suite *s;
    TCase *tc_core;

    s = suite_create("Clanchat");
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_error_if_name_empty);
    suite_add_tcase(s, tc_core);

    return s;
}

int main (void) {
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = clanchat_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
