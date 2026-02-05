#pragma once

#include "syscalls.h"
#include <definitions.h>
#include <calls.h>

static inline void* allocate(uint64_t amount)
{
    SYSCALL_1_RETURN(ALLOCATE, void*, amount);
}

static inline void* allocateAligned(uint64_t amount, uint64_t alignment)
{
    SYSCALL_2_RETURN(ALLOCATE_ALIGNED, void*, amount, alignment);
}

static inline void unallocate(void* pointer)
{
    SYSCALL_1(UNALLOCATE, pointer);
}
