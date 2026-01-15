#pragma once

#include <definitions.h>

#define DEFAULT "\xff\x00"
#define WHITE "\xff\x01"
#define BLUE "\xff\x02"
#define GREEN "\xff\x03"

void initTerminal();

void put(char character);

void write(const char* message);

void clear();

void read(char* buffer, uint64_t length);
