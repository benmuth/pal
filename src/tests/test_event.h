#include <stdio.h>
#include <stdbool.h>

#include "test.h"
#include <stdio.h>

// static int tests_run = 0;
// static int tests_passed = 0;
// static int assertions_failed = 0;
// static bool current_test_passed = true;


TEST(test_one) {
    ASSERT_EQ(1, 1);
}

int main(void) {
    RUN_TEST(test_one);
    printf("Tests run: %d\n", tests_run);  // Access the globals
    return 0;
}
