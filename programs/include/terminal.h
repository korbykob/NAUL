#pragma once

#include "syscalls.h"
#include <definitions.h>
#include <calls.h>

#define TERM_CLEAR "\xff"
#define TERM_DEFAULT "\xfe\x00"
#define TERM_WHITE "\xfe\x01"
#define TERM_BLUE "\xfe\x02"
#define TERM_GREEN "\xfe\x03"

static inline void put(char character)
{
    SYSCALL_1(PUT, character);
}

static inline void write(const char* message)
{
    SYSCALL_1(WRITE, message);
}

static inline void read(char* buffer, uint64_t length)
{
    SYSCALL_2(READ, buffer, length);
}
