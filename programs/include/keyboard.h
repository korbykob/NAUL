#pragma once

#include "syscalls.h"
#include <definitions.h>

typedef struct
{
    struct
    {
        uint8_t scancode;
        bool pressed;
    } buffer[256];
    uint8_t current;
} KeyboardBuffer;

static void registerKeyboard(KeyboardBuffer* buffer)
{
    SYSCALL_1(21, buffer);
}

static void unregisterKeyboard(KeyboardBuffer* buffer)
{
    SYSCALL_1(22, buffer);
}
