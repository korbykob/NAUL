#pragma once

#include "syscalls.h"
#include <definitions.h>

static uint64_t execute(const char* filename)
{
    SYSCALL_1_RETURN(0, uint64_t, filename);
}

static void quit()
{
    SYSCALL_0(1);
}
