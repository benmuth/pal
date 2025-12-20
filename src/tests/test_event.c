/* Test suite for event.c - Public API only
 * Tests the public interface defined in event.h
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Include pal headers - main.h defines translation macros
#include "../main.h"
#include "../event.h"

// Stub the gettext function (translation not needed for tests)
char *
gettext (const char *msgid)
{
  return (char *)msgid;
}

// Test framework
#include "test.h"

// Test tracking variables
static int tests_run = 0;
static int tests_passed = 0;
static int assertions_failed = 0;
static bool current_test_passed = true;

// ============================================================================
// Stubs for dependencies
// ============================================================================
// event.c references external symbols that are normally provided by main.c
// and add.c. When we compile event.c as a standalone object file for testing,
// we need to provide these symbols or the linker will fail with "undefined
// symbol" errors. These stubs provide minimal implementations just for testing.

// Global variables (normally defined in main.c)
Settings *settings = NULL;
GHashTable *ht = NULL;

// Helper function (normally defined in add.c)
// Converts numbers to ordinal suffixes: 1→"1st", 2→"2nd", 3→"3rd", etc.
// Used by event.c for describing monthly/yearly recurrence patterns
void
pal_add_suffix (gint number, gchar *suffix, gint buf_size)
{
  number = number % 10;
  switch (number)
    {
    case 1:
      snprintf (suffix, buf_size, "1st");
      return;
    case 2:
      snprintf (suffix, buf_size, "2nd");
      return;
    case 3:
      snprintf (suffix, buf_size, "3rd");
      return;
    default:
      snprintf (suffix, buf_size, "%dth", number);
      return;
    }
}

// Helper to initialize hash table for get_events tests
static void
setup_test_hashtable (void)
{
  if (ht != NULL)
    g_hash_table_destroy (ht);
  ht = g_hash_table_new (g_str_hash, g_str_equal);
}

// Helper to add an event to the hashtable
static void
add_test_event (const gchar *key, const gchar *text)
{
  PalEvent *event = pal_event_init ();
  event->text = g_strdup (text);
  event->key = g_strdup (key);

  GList *events = g_hash_table_lookup (ht, key);
  events = g_list_append (events, event);
  g_hash_table_insert (ht, g_strdup (key), events);
}

// ============================================================================
// TEST: pal_event_init / pal_event_free
// ============================================================================

TEST (test_pal_event_init_creates_zeroed_struct)
{
  PalEvent *event = pal_event_init ();

  ASSERT_NOT_NULL (event);
  ASSERT_NULL (event->text);
  ASSERT_NULL (event->type);
  ASSERT_NULL (event->start_date);
  ASSERT_NULL (event->end_date);
  ASSERT_NULL (event->date_string);
  ASSERT_NULL (event->start_time);
  ASSERT_NULL (event->end_time);
  ASSERT_NULL (event->file_name);
  ASSERT_NULL (event->key);
  ASSERT_NULL (event->eventtype);
  ASSERT_EQ (event->period_count, 1);

  pal_event_free (event);
}

TEST (test_pal_event_free_handles_null)
{
  // Should not crash
  pal_event_free (NULL);
  ASSERT_TRUE (true); // If we get here, it didn't crash
}

// ============================================================================
// TEST: get_date / get_key (conversion functions)
// ============================================================================

TEST (test_get_date_valid_dates)
{
  GDate *date;

  // Test Christmas 2024
  date = get_date ("20241225");
  ASSERT_NOT_NULL (date);
  ASSERT_EQ (g_date_get_day (date), 25);
  ASSERT_EQ (g_date_get_month (date), 12);
  ASSERT_EQ (g_date_get_year (date), 2024);
  g_date_free (date);

  // Test New Year 2000
  date = get_date ("20000101");
  ASSERT_NOT_NULL (date);
  ASSERT_EQ (g_date_get_day (date), 1);
  ASSERT_EQ (g_date_get_month (date), 1);
  ASSERT_EQ (g_date_get_year (date), 2000);
  g_date_free (date);

  // Test leap day
  date = get_date ("20000229");
  ASSERT_NOT_NULL (date);
  ASSERT_EQ (g_date_get_day (date), 29);
  ASSERT_EQ (g_date_get_month (date), 2);
  g_date_free (date);
}

TEST (test_get_date_with_zero_padding)
{
  // Test July 5 (single digit month and day)
  GDate *date = get_date ("20240705");

  ASSERT_NOT_NULL (date);
  ASSERT_EQ (g_date_get_day (date), 5);
  ASSERT_EQ (g_date_get_month (date), 7);
  ASSERT_EQ (g_date_get_year (date), 2024);

  g_date_free (date);
}

TEST (test_get_date_invalid_returns_null)
{
  // Invalid dates should return NULL
  ASSERT_NULL (get_date ("20241332")); // Month 13
  ASSERT_NULL (get_date ("20240230")); // Feb 30
  ASSERT_NULL (get_date ("20240431")); // April 31
  ASSERT_NULL (get_date ("00000000")); // All zeros
  ASSERT_NULL (get_date ("99999999")); // Invalid date
}

TEST (test_get_key_basic_format)
{
  GDate *date = g_date_new_dmy (25, 12, 2024);
  gchar *key = get_key (date);

  ASSERT_STR_EQ (key, "20241225");

  g_free (key);
  g_date_free (date);
}

TEST (test_get_key_zero_padding)
{
  GDate *date = g_date_new_dmy (5, 7, 2024);
  gchar *key = get_key (date);

  // Should be zero-padded: 2024-07-05
  ASSERT_STR_EQ (key, "20240705");

  g_free (key);
  g_date_free (date);
}

TEST (test_get_key_and_get_date_are_inverses)
{
  // Test roundtrip conversion
  GDate *original = g_date_new_dmy (15, 8, 2024);
  gchar *key = get_key (original);
  GDate *reconstructed = get_date (key);

  ASSERT_GDATE_EQ (original, reconstructed);

  g_free (key);
  g_date_free (original);
  g_date_free (reconstructed);
}

TEST (test_get_key_various_dates)
{
  GDate *date;
  gchar *key;

  // Test edge cases
  date = g_date_new_dmy (1, 1, 2000);
  key = get_key (date);
  ASSERT_STR_EQ (key, "20000101");
  g_free (key);
  g_date_free (date);

  date = g_date_new_dmy (31, 12, 9999);
  key = get_key (date);
  ASSERT_STR_EQ (key, "99991231");
  g_free (key);
  g_date_free (date);

  // Leap day
  date = g_date_new_dmy (29, 2, 2024);
  key = get_key (date);
  ASSERT_STR_EQ (key, "20240229");
  g_free (key);
  g_date_free (date);
}

// ============================================================================
// TEST: parse_event
// ============================================================================

TEST (test_parse_event_simple_daily)
{
  PalEvent *event = pal_event_init ();

  ASSERT_TRUE (parse_event (event, "DAILY"));
  ASSERT_NOT_NULL (event->key);
  ASSERT_STR_EQ (event->key, "DAILY");
  ASSERT_NOT_NULL (event->eventtype);
  ASSERT_EQ (event->period_count, 1);

  pal_event_free (event);
}

TEST (test_parse_event_one_time_date)
{
  PalEvent *event = pal_event_init ();

  ASSERT_TRUE (parse_event (event, "20241225"));
  ASSERT_STR_EQ (event->key, "20241225");
  ASSERT_NOT_NULL (event->eventtype);

  pal_event_free (event);
}

TEST (test_parse_event_weekly)
{
  PalEvent *event = pal_event_init ();

  ASSERT_TRUE (parse_event (event, "MON"));
  ASSERT_STR_EQ (event->key, "MON");

  pal_event_free (event);

  event = pal_event_init ();
  ASSERT_TRUE (parse_event (event, "FRI"));
  ASSERT_STR_EQ (event->key, "FRI");

  pal_event_free (event);
}

TEST (test_parse_event_with_date_range)
{
  PalEvent *event = pal_event_init ();

  // Format: KEY:STARTDATE:ENDDATE
  ASSERT_TRUE (parse_event (event, "DAILY:20240101:20241231"));
  ASSERT_STR_EQ (event->key, "DAILY");
  ASSERT_NOT_NULL (event->start_date);
  ASSERT_NOT_NULL (event->end_date);

  // Verify dates parsed correctly
  ASSERT_EQ (g_date_get_year (event->start_date), 2024);
  ASSERT_EQ (g_date_get_month (event->start_date), 1);
  ASSERT_EQ (g_date_get_day (event->start_date), 1);

  ASSERT_EQ (g_date_get_year (event->end_date), 2024);
  ASSERT_EQ (g_date_get_month (event->end_date), 12);
  ASSERT_EQ (g_date_get_day (event->end_date), 31);

  pal_event_free (event);
}

TEST (test_parse_event_with_start_date_only)
{
  PalEvent *event = pal_event_init ();

  ASSERT_TRUE (parse_event (event, "MON:20240101"));
  ASSERT_NOT_NULL (event->start_date);
  ASSERT_NOT_NULL (event->end_date); // Should default to year 3000

  ASSERT_EQ (g_date_get_year (event->end_date), 3000);

  pal_event_free (event);
}

TEST (test_parse_event_with_period_count)
{
  PalEvent *event = pal_event_init ();

  // Every 3rd day
  ASSERT_TRUE (parse_event (event, "DAILY/3:20240101:20241231"));
  ASSERT_EQ (event->period_count, 3);
  ASSERT_STR_EQ (event->key, "DAILY");

  pal_event_free (event);

  // Every 2nd occurrence
  event = pal_event_init ();
  ASSERT_TRUE (parse_event (event, "MON/2:20240101:20241231"));
  ASSERT_EQ (event->period_count, 2);

  pal_event_free (event);
}

TEST (test_parse_event_invalid_strings)
{
  PalEvent *event = pal_event_init ();

  // Invalid event type
  ASSERT_FALSE (parse_event (event, "INVALID"));

  // Invalid date
  ASSERT_FALSE (parse_event (event, "20241332"));

  // Empty string - SKIPPED: causes segfault
  // TODO: investigate why parse_event("") crashes
  ASSERT_FALSE (parse_event (event, ""));

  pal_event_free (event);
}

TEST (test_parse_event_invalid_date_range)
{
  PalEvent *event = pal_event_init ();

  // Invalid start date
  ASSERT_FALSE (parse_event (event, "DAILY:20241332:20241231"));

  // Invalid end date
  ASSERT_FALSE (parse_event (event, "DAILY:20240101:20241332"));

  pal_event_free (event);
}

TEST (test_parse_event_various_types)
{
  PalEvent *event;

  // TODO events
  event = pal_event_init ();
  ASSERT_TRUE (parse_event (event, "TODO"));
  ASSERT_STR_EQ (event->key, "TODO");
  pal_event_free (event);

  // Monthly (day of month)
  event = pal_event_init ();
  ASSERT_TRUE (parse_event (event, "00000015"));
  ASSERT_STR_EQ (event->key, "00000015");
  pal_event_free (event);

  // Yearly (month and day)
  event = pal_event_init ();
  ASSERT_TRUE (parse_event (event, "00001225"));
  ASSERT_STR_EQ (event->key, "00001225");
  pal_event_free (event);

  // Easter
  event = pal_event_init ();
  ASSERT_TRUE (parse_event (event, "EASTER"));
  ASSERT_STR_EQ (event->key, "EASTER");
  pal_event_free (event);

  // Easter with offset
  event = pal_event_init ();
  ASSERT_TRUE (parse_event (event, "EASTER+001"));
  ASSERT_STR_EQ (event->key, "EASTER+001");
  pal_event_free (event);
}

// ============================================================================
// TEST: pal_event_copy
// ============================================================================

TEST (test_pal_event_copy_basic)
{
  PalEvent *original = pal_event_init ();
  original->text = g_strdup ("Test event");
  original->type = g_strdup ("Birthday");
  original->file_name = g_strdup ("/tmp/test.pal");
  original->color = 3;
  original->file_num = 5;
  original->hide = TRUE;

  PalEvent *copy = pal_event_copy (original);

  ASSERT_STR_EQ (copy->text, "Test event");
  ASSERT_STR_EQ (copy->type, "Birthday");
  ASSERT_STR_EQ (copy->file_name, "/tmp/test.pal");
  ASSERT_EQ (copy->color, 3);
  ASSERT_EQ (copy->file_num, 5);
  ASSERT_EQ (copy->hide, TRUE);

  pal_event_free (original);
  pal_event_free (copy);
}

TEST (test_pal_event_copy_is_deep_copy)
{
  PalEvent *original = pal_event_init ();
  original->text = g_strdup ("Original text");

  PalEvent *copy = pal_event_copy (original);

  // Verify it's a deep copy (different pointers)
  ASSERT_TRUE (copy->text != original->text);

  // Modify original - should not affect copy
  g_free (original->text);
  original->text = g_strdup ("Modified");

  ASSERT_STR_EQ (copy->text, "Original text");

  pal_event_free (original);
  pal_event_free (copy);
}

TEST (test_pal_event_copy_with_dates)
{
  PalEvent *original = pal_event_init ();
  original->start_date = g_date_new_dmy (1, 1, 2024);
  original->end_date = g_date_new_dmy (31, 12, 2024);

  PalEvent *copy = pal_event_copy (original);

  ASSERT_NOT_NULL (copy->start_date);
  ASSERT_NOT_NULL (copy->end_date);
  ASSERT_GDATE_EQ (copy->start_date, original->start_date);
  ASSERT_GDATE_EQ (copy->end_date, original->end_date);

  // Verify deep copy
  ASSERT_TRUE (copy->start_date != original->start_date);
  ASSERT_TRUE (copy->end_date != original->end_date);

  pal_event_free (original);
  pal_event_free (copy);
}

TEST (test_pal_event_copy_with_times)
{
  PalEvent *original = pal_event_init ();
  original->start_time = g_malloc (sizeof (PalTime));
  original->start_time->hour = 14;
  original->start_time->min = 30;
  original->end_time = g_malloc (sizeof (PalTime));
  original->end_time->hour = 16;
  original->end_time->min = 45;

  PalEvent *copy = pal_event_copy (original);

  ASSERT_NOT_NULL (copy->start_time);
  ASSERT_NOT_NULL (copy->end_time);
  ASSERT_EQ (copy->start_time->hour, 14);
  ASSERT_EQ (copy->start_time->min, 30);
  ASSERT_EQ (copy->end_time->hour, 16);
  ASSERT_EQ (copy->end_time->min, 45);

  // Verify deep copy
  ASSERT_TRUE (copy->start_time != original->start_time);
  ASSERT_TRUE (copy->end_time != original->end_time);

  pal_event_free (original);
  pal_event_free (copy);
}

// ============================================================================
// TEST: pal_event_escape (age calculation)
// ============================================================================

TEST (test_pal_event_escape_simple_text)
{
  GDate *today = g_date_new_dmy (1, 1, 2024);
  PalEvent *event = pal_event_init ();
  event->text = g_strdup ("Simple event text");

  gchar *result = pal_event_escape (event, today);

  ASSERT_STR_EQ (result, "Simple event text");

  g_free (result);
  pal_event_free (event);
  g_date_free (today);
}

TEST (test_pal_event_escape_age_calculation)
{
  GDate *today = g_date_new_dmy (1, 1, 2024);
  PalEvent *event = pal_event_init ();
  event->text = g_strdup ("Birthday !2000! years old");

  gchar *result = pal_event_escape (event, today);

  // Should replace !2000! with 24 (2024 - 2000)
  ASSERT_NOT_NULL (strstr (result, "24"));
  ASSERT_NULL (strstr (result, "!2000!"));

  g_free (result);
  pal_event_free (event);
  g_date_free (today);
}

TEST (test_pal_event_escape_multiple_ages)
{
  GDate *today = g_date_new_dmy (15, 6, 2024);
  PalEvent *event = pal_event_init ();
  event->text = g_strdup ("Born !2000!, married !2020!");

  gchar *result = pal_event_escape (event, today);

  // 2024 - 2000 = 24
  ASSERT_NOT_NULL (strstr (result, "24"));
  // 2024 - 2020 = 4
  ASSERT_NOT_NULL (strstr (result, "4"));
  ASSERT_NULL (strstr (result, "!"));

  g_free (result);
  pal_event_free (event);
  g_date_free (today);
}

TEST (test_pal_event_escape_no_special_markers)
{
  GDate *today = g_date_new_dmy (1, 1, 2024);
  PalEvent *event = pal_event_init ();
  event->text = g_strdup ("Regular text with !incomplete");

  gchar *result = pal_event_escape (event, today);

  // Incomplete markers should be preserved
  ASSERT_NOT_NULL (strstr (result, "!incomplete"));

  g_free (result);
  pal_event_free (event);
  g_date_free (today);
}

// ============================================================================
// TEST: get_events (requires hashtable setup)
// ============================================================================

TEST (test_get_events_empty_hashtable)
{
  setup_test_hashtable ();

  GDate *date = g_date_new_dmy (1, 1, 2024);
  GList *events = get_events (date);

  ASSERT_NULL (events);

  g_date_free (date);
}

TEST (test_get_events_finds_daily_event)
{
  setup_test_hashtable ();
  add_test_event ("DAILY", "Daily standup");

  GDate *date = g_date_new_dmy (15, 6, 2024);
  GList *events = get_events (date);

  ASSERT_NOT_NULL (events);
  ASSERT_EQ (g_list_length (events), 1);

  PalEvent *event = (PalEvent *)events->data;
  ASSERT_STR_EQ (event->text, "Daily standup");

  g_list_free (events);
  g_date_free (date);
}

TEST (test_get_events_finds_specific_date)
{
  setup_test_hashtable ();
  add_test_event ("20241225", "Christmas");

  // Should find on Christmas
  GDate *christmas = g_date_new_dmy (25, 12, 2024);
  GList *events = get_events (christmas);
  ASSERT_NOT_NULL (events);
  ASSERT_EQ (g_list_length (events), 1);
  g_list_free (events);
  g_date_free (christmas);

  // Should NOT find on other day
  GDate *other = g_date_new_dmy (26, 12, 2024);
  events = get_events (other);
  ASSERT_NULL (events);
  g_date_free (other);
}

TEST (test_pal_get_event_count)
{
  setup_test_hashtable ();
  add_test_event ("DAILY", "Event 1");
  add_test_event ("DAILY", "Event 2");

  GDate *date = g_date_new_dmy (1, 1, 2024);
  gint count = pal_get_event_count (date);

  ASSERT_EQ (count, 2);

  g_date_free (date);
}

TEST (test_pal_get_event_count_empty)
{
  setup_test_hashtable ();

  GDate *date = g_date_new_dmy (1, 1, 2024);
  gint count = pal_get_event_count (date);

  ASSERT_EQ (count, 0);

  g_date_free (date);
}

// ============================================================================
// MAIN TEST RUNNER
// ============================================================================

int
main (void)
{
  // Initialize global settings
  settings = g_malloc (sizeof (Settings));
  settings->date_fmt = g_strdup ("%a %b %d, %Y");
  ht = NULL;

  printf ("Running event.c public API tests...\n\n");

  // Test event lifecycle
  printf ("=== EVENT LIFECYCLE ===\n");
  RUN_TEST (test_pal_event_init_creates_zeroed_struct);
  RUN_TEST (test_pal_event_free_handles_null);

  // Test date/key conversion
  printf ("\n=== DATE/KEY CONVERSION ===\n");
  RUN_TEST (test_get_date_valid_dates);
  RUN_TEST (test_get_date_with_zero_padding);
  RUN_TEST (test_get_date_invalid_returns_null);
  RUN_TEST (test_get_key_basic_format);
  RUN_TEST (test_get_key_zero_padding);
  RUN_TEST (test_get_key_and_get_date_are_inverses);
  RUN_TEST (test_get_key_various_dates);

  // Test event parsing
  printf ("\n=== EVENT PARSING ===\n");
  RUN_TEST (test_parse_event_simple_daily);
  RUN_TEST (test_parse_event_one_time_date);
  RUN_TEST (test_parse_event_weekly);
  RUN_TEST (test_parse_event_with_date_range);
  RUN_TEST (test_parse_event_with_start_date_only);
  RUN_TEST (test_parse_event_with_period_count);
  RUN_TEST (test_parse_event_invalid_strings);
  RUN_TEST (test_parse_event_invalid_date_range);
  RUN_TEST (test_parse_event_various_types);

  // Test event copying
  printf ("\n=== EVENT COPYING ===\n");
  RUN_TEST (test_pal_event_copy_basic);
  RUN_TEST (test_pal_event_copy_is_deep_copy);
  RUN_TEST (test_pal_event_copy_with_dates);
  RUN_TEST (test_pal_event_copy_with_times);

  // Test text escaping
  printf ("\n=== TEXT ESCAPING (AGE CALCULATION) ===\n");
  RUN_TEST (test_pal_event_escape_simple_text);
  RUN_TEST (test_pal_event_escape_age_calculation);
  RUN_TEST (test_pal_event_escape_multiple_ages);
  RUN_TEST (test_pal_event_escape_no_special_markers);

  // Test event retrieval
  printf ("\n=== EVENT RETRIEVAL ===\n");
  RUN_TEST (test_get_events_empty_hashtable);
  RUN_TEST (test_get_events_finds_daily_event);
  RUN_TEST (test_get_events_finds_specific_date);
  RUN_TEST (test_pal_get_event_count);
  RUN_TEST (test_pal_get_event_count_empty);

  // Print summary
  printf ("\n");
  printf ("=================================\n");
  printf ("Tests run:         %d\n", tests_run);
  printf ("Tests passed:      %d\n", tests_passed);
  printf ("Tests failed:      %d\n", tests_run - tests_passed);
  printf ("Assertions failed: %d\n", assertions_failed);
  printf ("=================================\n");

  // Cleanup
  if (ht != NULL)
    g_hash_table_destroy (ht);
  g_free (settings->date_fmt);
  g_free (settings);

  return (tests_passed == tests_run) ? 0 : 1;
}
