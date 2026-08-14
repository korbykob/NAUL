#pragma once

#include "syscalls.h"
#include <calls.h>

static inline void reboot()
{
    SYSCALL_0(REBOOT);
}

static inline void shutdown()
{
    SYSCALL_0(SHUTDOWN);
}
