#pragma once

#include <definitions.h>

#define TERM_DEFAULT "\xff\x00"
#define TERM_WHITE "\xff\x01"
#define TERM_BLUE "\xff\x02"
#define TERM_GREEN "\xff\x03"

void initTerminal();

void put(char character);

void write(const char* message);

void clear();

void read(char* buffer, uint64_t length);
