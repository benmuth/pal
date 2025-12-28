/*
 * Unity build file for pal
 *
 * This file combines all source files into a single translation unit,
 * enabling better compiler optimizations and faster build times.
 *
 * To build with unity build:
 *   make UNITY=1
 *
 * To build traditionally:
 *   make
 */

/* Core initialization and globals */
#include "main.c"

/* Utility modules */
#include "colorize.c"
#include "output.c"
#include "input.c"
#include "event.c"
#include "rl.c"

/* Export modules */
#include "html.c"

/* Interactive mode modules */
#include "add.c"
#include "edit.c"
#include "del.c"
#include "remind.c"
#include "search.c"
#include "manage.c"
