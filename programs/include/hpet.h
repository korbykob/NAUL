#pragma once

#include "syscalls.h"
#include <definitions.h>
#include <calls.h>

#define FEMTOSECONDS_PER_SECOND 1000000000000000
#define FEMTOSECONDS_PER_MILLISECOND 1000000000000
#define FEMTOSECONDS_PER_MICROSECOND 1000000000

static inline uint64_t getFemtoseconds()
{
    SYSCALL_0_RETURN(GET_FEMTOSECONDS, uint64_t);
}

static inline void waitFemtoseconds(uint64_t femtoseconds)
{
    SYSCALL_1(WAIT_FEMTOSECONDS, femtoseconds);
}

static inline void cancelWait(uint64_t id)
{
    SYSCALL_1(CANCEL_WAIT, id);
}
