#pragma once

#include "syscalls.h"
#include <definitions.h>
#include <calls.h>

static inline void registerListener(uint64_t (*handler)(uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4), const char* name)
{
    SYSCALL_2(REGISTER_LISTENER, handler, name);
}

static inline void unregisterListener(uint64_t (*handler)(uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4))
{
    SYSCALL_1(UNREGISTER_LISTENER, handler);
}

static inline bool checkListener(const char* name)
{
    SYSCALL_1_RETURN(CHECK_LISTENER, bool, name);
}

static inline uint64_t sendMessage(const char* name, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4)
{
    SYSCALL_5_RETURN(SEND_MESSAGE, uint64_t, name, arg1, arg2, arg3, arg4);
}
