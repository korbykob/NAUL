#pragma once

#include <definitions.h>

void initSymbols();

uint64_t getOffset();

const char* getSymbol(uint64_t address, uint64_t* offset);
