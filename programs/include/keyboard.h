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
        uint8_t scancode;
        bool pressed;
    } buffer[256];
} KeyboardBuffer;

static inline void registerKeyboard(KeyboardBuffer* buffer)
{
    SYSCALL_1(REGISTER_KEYBOARD, buffer);
}

static inline void unregisterKeyboard(KeyboardBuffer* buffer)
{
    SYSCALL_1(UNREGISTER_KEYBOARD, buffer);
}
