#pragma once

#include "syscalls.h"
#include <definitions.h>

typedef struct
{
    uint32_t* buffer;
    uint32_t width;
    uint32_t height;
} Display;

static void obtainDisplay(Display* display)
{
    SYSCALL_1(23, display);
}

static void releaseDisplay()
{
    SYSCALL_0(24);
}
