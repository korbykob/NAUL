#pragma once

#include "syscalls.h"
#include <definitions.h>
#include <calls.h>

#define yieldThread() __asm__ volatile ("int $0x67")

static inline uint64_t createThread(void (*function)())
{
    SYSCALL_1_RETURN(CREATE_THREAD, uint64_t, function);
}

static inline void waitForThread(uint64_t id)
{
    SYSCALL_1(WAIT_FOR_THREAD, id);
}

static inline void destroyThread(uint64_t id)
{
    SYSCALL_1(DESTROY_THREAD, id);
}

static inline void exitThread()
{
    SYSCALL_0(EXIT_THREAD);
}
