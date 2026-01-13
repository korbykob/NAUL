#pragma once

#include <definitions.h>

static void setMemory(void* destination, uint8_t value, uint64_t count)
{
    __asm__ volatile ("rep stosb" : "+D"(destination), "+c"(count) : "a"(value) : "memory");
}

static void copyMemory(void* source, void* destination, uint64_t count)
{
    __asm__ volatile ("rep movsb" : "+S"(source), "+D"(destination), "+c"(count) : : "memory");
}
