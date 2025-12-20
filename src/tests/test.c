#include "test.h"
#include <stdio.h>

int
main (void)
{
  printf ("Running tests...\n\n");

  // RUN_TEST (test_addition);
  // RUN_TEST (test_strings);
  // RUN_TEST (test_with_multiple_failures);

  printf ("\n");
  printf ("=================================\n");
  printf ("Tests run:    %d\n", tests_run);
  printf ("Tests passed: %d\n", tests_passed);
  printf ("Tests failed: %d\n", tests_run - tests_passed);
  printf ("Assertions failed: %d\n", assertions_failed);
  printf ("=================================\n");

  return (tests_passed == tests_run) ? 0 : 1;
}

