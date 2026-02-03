#pragma once

#include "syscalls.h"
#include <definitions.h>

typedef struct
{
    struct
    {
        int16_t x;
        int16_t y;
        bool left;
        bool right;
    } buffer[256];
    uint8_t current;
} MouseBuffer;

static void registerMouse(MouseBuffer* buffer)
{
    SYSCALL_1(23, buffer);
}

static void unregisterMouse(MouseBuffer* buffer)
{
    SYSCALL_1(24, buffer);
}
