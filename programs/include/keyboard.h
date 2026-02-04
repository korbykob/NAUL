#pragma once

#include "syscalls.h"
#include <definitions.h>
#include <calls.h>

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
    SYSCALL_1(REGISTER_KEYBOARD, buffer);
}

static void unregisterKeyboard(KeyboardBuffer* buffer)
{
    SYSCALL_1(UNREGISTER_KEYBOARD, buffer);
}
