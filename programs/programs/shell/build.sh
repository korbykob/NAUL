#!/bin/bash
set -e

mkdir -p bin
gcc $PROGRAM_COMPILER_FLAGS src/shell.c -o bin/shell.o
nm bin/shell.o > bin/shell.sym
ld $PROGRAM_LINKER_FLAGS bin/shell.o -o bin/shell.bin
