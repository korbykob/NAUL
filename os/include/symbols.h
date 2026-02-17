#pragma once

#include <definitions.h>

typedef struct
{
    uint64_t address;
    char name[256];
} Symbol;

void initSymbols();

uint64_t getKernelOffset();

Symbol* parseSymbols(const char* file, uint64_t* count);

const char* getKernelSymbol(uint64_t address, uint64_t* offset);

const char* getSymbol(Symbol* symbols, uint64_t count, uint64_t address, uint64_t* offset);
