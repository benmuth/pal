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

# Check for vendored dependencies
if [ -d ../vendor/build ]; then
    VENDOR_PREFIX="$(cd .. && pwd)/vendor/build"
    echo "Using vendored dependencies from $VENDOR_PREFIX"
    USING_VENDOR=1
else
    echo "Using system libraries"
    USING_VENDOR=0
fi

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
if [ "$USING_VENDOR" = "1" ]; then
    # Use vendored libraries
    INCLUDE_FLAGS="-I$VENDOR_PREFIX/include -I$VENDOR_PREFIX/include/glib-2.0 -I$VENDOR_PREFIX/lib/glib-2.0/include -I$VENDOR_PREFIX/include/ncursesw"
    LINK_FLAGS="$VENDOR_PREFIX/lib/libglib-2.0.a $VENDOR_PREFIX/lib/libintl.a $VENDOR_PREFIX/lib/libpcre2-8.a $VENDOR_PREFIX/lib/libreadline.a $VENDOR_PREFIX/lib/libncursesw.a -lm -lpthread"

    # macOS needs additional libraries
    if [ "$(uname -s)" = "Darwin" ]; then
        LINK_FLAGS="$LINK_FLAGS -liconv -framework Foundation"
    fi
else
    # Use system libraries
    INCLUDE_FLAGS="-I${PREFIX}/include $(pkg-config --cflags glib-2.0)"
    LINK_FLAGS="$(pkg-config --libs glib-2.0) -lreadline -lncurses"
fi

$CC $CFLAGS \
    $INCLUDE_FLAGS \
    -DPAL_VERSION=\"$VERSION\" \
    -DPREFIX=\"$PREFIX\" \
    $SOURCES \
    $LINK_FLAGS \
    -o pal

# Strip if optimized
if [ "$DEBUG" = "0" ]; then
    strip pal
fi

echo "Build complete: ./pal"
