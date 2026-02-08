#pragma once

#include <definitions.h>

#define TERM_CLEAR '\xff'
#define TERM_DEFAULT "\xfe\x00"
#define TERM_WHITE "\xfe\x01"
#define TERM_BLUE "\xfe\x02"
#define TERM_GREEN "\xfe\x03"

void initTerminal();

void redrawTerminal();

void put(char character);

void write(const char* message);

void read(char* buffer, uint64_t length);
