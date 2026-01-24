#pragma once

#include "syscalls.h"
#include <definitions.h>

#define femtosecondsPerSecond 1000000000000000
#define femtosecondsPerMicrosecond 1000000000

static uint64_t getFemtoseconds()
{
    SYSCALL_0_RETURN(16, uint64_t);
}
