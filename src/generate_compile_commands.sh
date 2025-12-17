#!/bin/bash
# Generate compile_commands.json for clangd

SRCDIR="$(pwd)"
PREFIX="$HOME/.local"

# Get compiler flags from Makefile (escape quotes for JSON)
CFLAGS="-O2 -Wall -I${PREFIX}/include $(pkg-config --cflags glib-2.0) -DPAL_VERSION=\\\"0.4.2\\\" -DPREFIX=\\\"${PREFIX}\\\""

# Source files from Makefile
SOURCES=(
    main.c colorize.c output.c input.c event.c rl.c html.c latex.c
    add.c edit.c del.c remind.c search.c manage.c
)

# Generate JSON
echo '['
for i in "${!SOURCES[@]}"; do
    src="${SOURCES[$i]}"
    
    # Add comma except for last entry
    if [ $i -eq $((${#SOURCES[@]} - 1)) ]; then
        comma=""
    else
        comma=","
    fi
    
    cat << JSON
  {
    "directory": "${SRCDIR}",
    "command": "gcc ${CFLAGS} -c ${src} -o tmp.opt/${src%.c}.o",
    "file": "${SRCDIR}/${src}"
  }${comma}
JSON
done
echo ']'
