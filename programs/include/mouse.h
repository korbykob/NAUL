#pragma once

#include "syscalls.h"
#include <definitions.h>
#include <calls.h>

typedef struct
{
    uint8_t head;
    uint8_t tail;
    struct
    {
        int16_t x;
        int16_t y;
        bool left;
        bool right;
    } buffer[256];
} MouseBuffer;

static inline void registerMouse(MouseBuffer* buffer)
{
    SYSCALL_1(REGISTER_MOUSE, buffer);
}

static inline void unregisterMouse(MouseBuffer* buffer)
{
    SYSCALL_1(UNREGISTER_MOUSE, buffer);
}
