#pragma once

#include "syscalls.h"

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

static void read(char* buffer)
{
    SYSCALL_1(5, buffer);
}
