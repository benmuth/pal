#!/bin/sh
# Build script for pal
#
# Usage:
#   ./build.sh          # Traditional build (optimized)
#   ./build.sh --unity  # Unity build (optimized)
#   ./build.sh --debug  # Debug mode

set -e

# Parse arguments
UNITY=0
DEBUG=0

for arg in "$@"; do
    case "$arg" in
        --unity) UNITY=1 ;;
        --debug) DEBUG=1 ;;
        *) echo "Usage: $0 [--unity] [--debug]"; exit 1 ;;
    esac
done

# Configuration
PREFIX="${PREFIX:-$HOME/.local}"
VERSION="0.4.2"
CC="${CC:-gcc}"

# Compiler flags
if [ "$DEBUG" = "1" ]; then
    CFLAGS="-g -Wall -pedantic -Wstrict-prototypes -DG_DISABLE_DEPRECATED -DDEBUG"
else
    CFLAGS="-O2 -Wall"
fi

# Source files
if [ "$UNITY" = "1" ]; then
    SOURCES="pal_unity.c"
    echo "=== Unity Build ==="
else
    SOURCES="main.c colorize.c output.c input.c event.c rl.c html.c add.c edit.c del.c remind.c search.c manage.c"
    echo "=== Traditional Build ==="
fi

# Compile
echo "Compiling..."
$CC $CFLAGS \
    -I${PREFIX}/include \
    $(pkg-config --cflags glib-2.0) \
    -DPAL_VERSION=\"$VERSION\" \
    -DPREFIX=\"$PREFIX\" \
    $SOURCES \
    $(pkg-config --libs glib-2.0) \
    -lreadline -lncurses \
    -o pal

# Strip if optimized
if [ "$DEBUG" = "0" ]; then
    strip pal
fi

echo "Build complete: ./pal"
