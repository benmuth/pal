#include <stdbool.h>
#include <stdio.h>
#include <string.h>

// Test tracking
// Include these declarations in every test file
// static int tests_run = 0;
// static int tests_passed = 0;
// static int assertions_failed = 0;
// static bool current_test_passed = true;

// defines functions to run a test
#define TEST(name)                                                            \
  static void name (void);                                                    \
  static void run_##name (void)                                               \
  {                                                                           \
    current_test_passed = true;                                               \
    tests_run++;                                                              \
    name ();                                                                  \
    if (current_test_passed)                                                  \
      {                                                                       \
        tests_passed++;                                                       \
        printf ("  PASS: %s\n", #name);                                       \
      }                                                                       \
    else                                                                      \
      {                                                                       \
        printf ("  FAIL: %s\n", #name);                                       \
      }                                                                       \
  }                                                                           \
  static void name (void)

// calls the test function defined in TEST
#define RUN_TEST(name) run_##name ()

// macros used inside a TEST to evaluate expressions

#define ASSERT_TRUE(expr)                                                     \
  do                                                                          \
    {                                                                         \
      if (!(expr))                                                            \
        {                                                                     \
          printf ("    FAIL: %s:%d - Expected true: %s\n", __FILE__,          \
                  __LINE__, #expr);                                           \
          current_test_passed = false;                                        \
          assertions_failed++;                                                \
        }                                                                     \
    }                                                                         \
  while (0)

#define ASSERT_FALSE(expr)                                                    \
  do                                                                          \
    {                                                                         \
      if (expr)                                                               \
        {                                                                     \
          printf ("    FAIL: %s:%d - Expected false: %s\n", __FILE__,         \
                  __LINE__, #expr);                                           \
          current_test_passed = false;                                        \
          assertions_failed++;                                                \
        }                                                                     \
    }                                                                         \
  while (0)

#define ASSERT_EQ(a, b)                                                       \
  do                                                                          \
    {                                                                         \
      if ((a) != (b))                                                         \
        {                                                                     \
          printf ("    FAIL: %s:%d - Expected %s == %s\n", __FILE__,          \
                  __LINE__, #a, #b);                                          \
          current_test_passed = false;                                        \
          assertions_failed++;                                                \
        }                                                                     \
    }                                                                         \
  while (0)

#define ASSERT_STR_EQ(a, b)                                                   \
  do                                                                          \
    {                                                                         \
      if (strcmp ((a), (b)) != 0)                                             \
        {                                                                     \
          printf ("    FAIL: %s:%d - Expected \"%s\" == \"%s\" (got \"%s\" "  \
                  "vs \"%s\")\n",                                             \
                  __FILE__, __LINE__, #a, #b, (a), (b));                      \
          current_test_passed = false;                                        \
          assertions_failed++;                                                \
        }                                                                     \
    }                                                                         \
  while (0)

#define ASSERT_NULL(expr)                                                     \
  do                                                                          \
    {                                                                         \
      if ((expr) != NULL)                                                     \
        {                                                                     \
          printf ("    FAIL: %s:%d - Expected NULL: %s\n", __FILE__,          \
                  __LINE__, #expr);                                           \
          current_test_passed = false;                                        \
          assertions_failed++;                                                \
        }                                                                     \
    }                                                                         \
  while (0)

#define ASSERT_NOT_NULL(expr)                                                 \
  do                                                                          \
    {                                                                         \
      if ((expr) == NULL)                                                     \
        {                                                                     \
          printf ("    FAIL: %s:%d - Expected non-NULL: %s\n", __FILE__,      \
                  __LINE__, #expr);                                           \
          current_test_passed = false;                                        \
          assertions_failed++;                                                \
        }                                                                     \
    }                                                                         \
  while (0)

// For comparing GDates (requires glib)
#define ASSERT_GDATE_EQ(a, b)                                                 \
  do                                                                          \
    {                                                                         \
      if (g_date_compare ((a), (b)) != 0)                                     \
        {                                                                     \
          gchar buf_a[32], buf_b[32];                                         \
          g_date_strftime (buf_a, 32, "%Y-%m-%d", (a));                       \
          g_date_strftime (buf_b, 32, "%Y-%m-%d", (b));                       \
          printf ("    FAIL: %s:%d - Dates not equal: %s vs %s\n", __FILE__,  \
                  __LINE__, buf_a, buf_b);                                    \
          current_test_passed = false;                                        \
          assertions_failed++;                                                \
        }                                                                     \
    }                                                                         \
  while (0)

// // Example tests
// TEST (test_addition)
// {
//   ASSERT_EQ (2 + 2, 4);
//   ASSERT_EQ (10 - 5, 5);
//   ASSERT_TRUE (5 > 3);
// }

// TEST (test_strings)
// {
//   ASSERT_STR_EQ ("hello", "hello");
//   ASSERT_FALSE (strcmp ("foo", "bar") == 0);
// }

// TEST (test_with_multiple_failures)
// {
//   ASSERT_EQ (1, 2);    // Fails but test continues
//   ASSERT_EQ (3, 3);    // Still runs
//   ASSERT_TRUE (false); // Also runs
// }

