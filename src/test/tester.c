#include "test/tester.h"
#include <stdio.h>

const char *kSuccess = "\x1b[0m[\x1b[32mOK\x1b[0m]";
const char *kFailure = "\x1b[0m[\x1b[31mXX\x1b[0m]";

void testmain(test_t *test) {
    int n_success = 0;
    int n_failed = 0;

    while (test->func) {
        if (test->func()) {
            n_failed++;
            printf(" %s %s\n", kFailure, test->desc);
        } else {
            n_success++;
            printf(" %s %s\n", kSuccess, test->desc);
        }

        test++;
    }

    printf("\n%d success, %d failure\n", n_success, n_failed);
}
