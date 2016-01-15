#include <check.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include <curl/curl.h>

#include "../src/clanchat.h"
#include "../src/clanchatapi.h"

START_TEST(test_responds_with_200_success)
{
    CURL *curl;
    curl = curl_easy_init();
    ck_assert_msg (curl, "Curl failed to load.");

    curl_easy_setopt(curl, CURLOPT_URL, "http://127.0.0.1:37777/");
    
    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 1L);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 1L);
    
    int retval = curl_easy_perform(curl);

    long response_code;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
    curl_easy_cleanup(curl);

    //ck_assert_msg (retval == CURLE_OK, "Curl failed");
    ck_assert_msg (response_code == 200L, "Bad response code from");
}
END_TEST

Suite * clanchatapi_suite (void) {
    Suite *s;
    TCase *tc_core;

    s = suite_create("Clanchat API");
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_responds_with_200_success);
    suite_add_tcase(s, tc_core);

    return s;
}

int main (void) {
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = clanchatapi_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

