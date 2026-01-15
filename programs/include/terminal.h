#pragma once

#include "syscalls.h"
#include <definitions.h>

#define DEFAULT "\xff\x00"
#define WHITE "\xff\x01"
#define BLUE "\xff\x02"
#define GREEN "\xff\x03"

static void put(char character)
{
    SYSCALL_1(2, character);
}

static void write(const char* message)
{
    SYSCALL_1(3, message);
}

static void clear()
{
    SYSCALL_0(4);
}

static void read(char* buffer, uint64_t length)
{
    SYSCALL_2(5, buffer, length);
}
