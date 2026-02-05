#pragma once

#include "syscalls.h"
#include <definitions.h>
#include <calls.h>

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

static inline void registerMouse(MouseBuffer* buffer)
{
    SYSCALL_1(REGISTER_MOUSE, buffer);
}

static inline void unregisterMouse(MouseBuffer* buffer)
{
    SYSCALL_1(UNREGISTER_MOUSE, buffer);
}
