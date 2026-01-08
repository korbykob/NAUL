#pragma once

#include "syscalls.h"
#include <definitions.h>

static void* allocate(uint64_t amount)
{
    SYSCALL_1_RETURN(6, void*, amount);
}

static void* allocateAligned(uint64_t amount, uint64_t alignment)
{
    SYSCALL_2_RETURN(7, void*, amount, alignment);
}

static void unallocate(void* pointer)
{
    SYSCALL_1(8, pointer);
}
