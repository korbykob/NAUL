#!/bin/bash
set -e

mkdir -p bin
x86_64-linux-gnu-gcc $PROGRAM_COMPILER_FLAGS -g -fno-omit-frame-pointer -Wall -Wextra -Werror src/test.c -o bin/test.o
x86_64-linux-gnu-nm bin/test.o > bin/test.sym
x86_64-linux-gnu-ld $PROGRAM_LINKER_FLAGS bin/test.o -o bin/test.nxe
