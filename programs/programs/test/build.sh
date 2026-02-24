#!/bin/bash
set -e

mkdir -p bin
gcc $PROGRAM_COMPILER_FLAGS src/test.c -o bin/test.o
nm bin/test.o > bin/test.sym
ld $PROGRAM_LINKER_FLAGS bin/test.o -o bin/test.bin
