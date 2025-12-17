#!/bin/bash
set -e  # Exit on error

# Parse arguments
DEBUG=0
if [[ "$1" == "--debug" ]]; then
    DEBUG=1
fi

# Configuration
PREFIX="$HOME/.local"
VERSION="0.4.2"

# Compiler flags
if [[ $DEBUG -eq 1 ]]; then
    CFLAGS="-g -Wall -pedantic -Wstrict-prototypes -DG_DISABLE_DEPRECATED -DDEBUG"
    echo "Building in DEBUG mode..."
else
    CFLAGS="-O2 -Wall"
    echo "Building in OPTIMIZED mode..."
fi

# Get dependency flags
GLIB_CFLAGS=$(pkg-config --cflags glib-2.0)
GLIB_LIBS=$(pkg-config --libs glib-2.0)

# Source files
SOURCES="main.c colorize.c output.c input.c event.c rl.c html.c latex.c add.c edit.c del.c remind.c search.c manage.c"

# Compile and link in one step
echo "Compiling..."
gcc $CFLAGS \
    -I${PREFIX}/include \
    $GLIB_CFLAGS \
    -DPAL_VERSION=\"$VERSION\" \
    -DPREFIX=\"$PREFIX\" \
    $SOURCES \
    $GLIB_LIBS \
    -lreadline -lncurses \
    -o pal

# Strip if optimized
if [[ $DEBUG -eq 0 ]]; then
    echo "Stripping..."
    strip pal
fi

echo "Build complete: ./pal"
