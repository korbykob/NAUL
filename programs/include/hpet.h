#pragma once

#include "syscalls.h"
#include <definitions.h>
#include <calls.h>

#define femtosecondsPerSecond 1000000000000000
#define femtosecondsPerMicrosecond 1000000000

static inline uint64_t getFemtoseconds()
{
    SYSCALL_0_RETURN(GET_FEMTOSECONDS, uint64_t);
}
