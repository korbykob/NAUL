#pragma once

#include "syscalls.h"
#include <definitions.h>
#include <calls.h>

static void* allocate(uint64_t amount)
{
    SYSCALL_1_RETURN(ALLOCATE, void*, amount);
}

static void* allocateAligned(uint64_t amount, uint64_t alignment)
{
    SYSCALL_2_RETURN(ALLOCATE_ALIGNED, void*, amount, alignment);
}

static void unallocate(void* pointer)
{
    SYSCALL_1(UNALLOCATE, pointer);
}
