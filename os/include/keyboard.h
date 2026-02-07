#pragma once

#include <definitions.h>

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

void initKeyboard();

void registerKeyboard(KeyboardBuffer* buffer);

void unregisterKeyboard(KeyboardBuffer* buffer);
