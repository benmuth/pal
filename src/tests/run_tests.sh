#!/bin/bash
# Build and run test suite for pal

set -e  # Exit on error

echo "Building test_event..."

# Compile event.c as an object file
# Note: event.c includes main.h which defines Settings, PalEvent, etc.
# We need to compile with main.h available
clang -c -g \
    $(pkg-config --cflags glib-2.0) \
    -I.. \
    -DPAL_VERSION=\"0.4.2\" \
    -DPREFIX=\"$HOME/.local\" \
    -include ../main.h \
    ../event.c \
    -o event_test.o

# Compile and link the test file with event.o
clang -g \
    $(pkg-config --cflags glib-2.0) \
    -I.. \
    test_event.c \
    event_test.o \
    $(pkg-config --libs glib-2.0) \
    -o test_event

echo "Running tests..."
echo ""

./test_event

exit $?
