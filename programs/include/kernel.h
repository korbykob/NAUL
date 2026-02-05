#pragma once

#include "syscalls.h"
#include <definitions.h>
#include <calls.h>

static inline uint64_t execute(const char* filename)
{
    SYSCALL_1_RETURN(EXECUTE, uint64_t, filename);
}

static inline void quit()
{
    SYSCALL_0(QUIT);
}
