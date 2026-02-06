#pragma once

#include <definitions.h>

#define EXCEPTION_COUNT 32
#define JMP_SIZE 5

typedef struct 
{
    const char* name;
    bool code;
} Exception;

extern const Exception exceptions[EXCEPTION_COUNT];

void initPanic();

void panic(uint8_t exception, uint32_t code);
