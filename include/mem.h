#pragma once

#include <definitions.h>

static void setMemory8(void* destination, uint8_t value, uint64_t count)
{
    __asm__ volatile ("rep stosb" : "+D"(destination), "+c"(count) : "a"(value) : "memory");
}

static void setMemory16(void* destination, uint16_t value, uint64_t count)
{
    __asm__ volatile ("rep stosw" : "+D"(destination), "+c"(count) : "a"(value) : "memory");
}

static void setMemory32(void* destination, uint32_t value, uint64_t count)
{
    __asm__ volatile ("rep stosl" : "+D"(destination), "+c"(count) : "a"(value) : "memory");
}

static void setMemory64(void* destination, uint64_t value, uint64_t count)
{
    __asm__ volatile ("rep stosq" : "+D"(destination), "+c"(count) : "a"(value) : "memory");
}

static void copyMemory8(void* source, void* destination, uint64_t count)
{
    __asm__ volatile ("rep movsb" : "+S"(source), "+D"(destination), "+c"(count) : : "memory");
}

static void copyMemory16(void* source, void* destination, uint64_t count)
{
    __asm__ volatile ("rep movsw" : "+S"(source), "+D"(destination), "+c"(count) : : "memory");
}

static void copyMemory32(void* source, void* destination, uint64_t count)
{
    __asm__ volatile ("rep movsl" : "+S"(source), "+D"(destination), "+c"(count) : : "memory");
}

static void copyMemory64(void* source, void* destination, uint64_t count)
{
    __asm__ volatile ("rep movsq" : "+S"(source), "+D"(destination), "+c"(count) : : "memory");
}
