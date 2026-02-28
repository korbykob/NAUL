#pragma once

#include "syscalls.h"
#include <definitions.h>
#include <calls.h>

static inline void registerListener(uint64_t (*handler)(uint64_t arg1), const char* name)
{
    SYSCALL_2(REGISTER_LISTENER, handler, name);
}

static inline void unregisterListener(uint64_t (*handler)(uint64_t arg1))
{
    SYSCALL_1(UNREGISTER_LISTENER, handler);
}

static inline uint64_t sendMessage(const char* name, uint64_t arg1)
{
    SYSCALL_2_RETURN(SEND_MESSAGE, uint64_t, name, arg1);
}
