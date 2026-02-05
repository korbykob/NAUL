#pragma once

#include "syscalls.h"
#include <definitions.h>
#include <calls.h>

typedef struct
{
    uint32_t* buffer;
    uint32_t width;
    uint32_t height;
} Display;

static inline void obtainDisplay(Display* display)
{
    SYSCALL_1(OBTAIN_DISPLAY, display);
}

static inline void releaseDisplay()
{
    SYSCALL_0(RELEASE_DISPLAY);
}
