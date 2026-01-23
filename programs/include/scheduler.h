#pragma once

#include "syscalls.h"
#include <definitions.h>

static uint64_t getMilliseconds()
{
    SYSCALL_0_RETURN(16, uint64_t);
}

static uint64_t createThread(void (*function)())
{
    SYSCALL_1_RETURN(17, uint64_t, function);
}

static void waitForThread(uint64_t id)
{
    SYSCALL_1(18, id);
}

static void destroyThread(uint64_t id)
{
    SYSCALL_1(19, id);
}

static void exitThread()
{
    SYSCALL_0(20);
}

static void yieldThread()
{
    __asm__ volatile ("int $0x67");
}
