#pragma once

#include <definitions.h>

void initTerminal(uint32_t* buffer, uint32_t width, uint32_t height, uint8_t* font);

void put(char character);

void write(const char* message);

void clear();

void read(char* buffer);
