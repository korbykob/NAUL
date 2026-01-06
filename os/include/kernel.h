#pragma once

#include <definitions.h>

typedef struct 
{
    const char* name;
    bool code;
} Exception;

extern const Exception exceptions[32];

void panic(uint8_t exception, uint32_t code, uint64_t address);

void kernel();
