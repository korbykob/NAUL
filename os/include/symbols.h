#pragma once

#include <definitions.h>

void initSymbols();

const char* getSymbol(uint64_t address, uint64_t* offset);
