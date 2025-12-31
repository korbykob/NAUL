#pragma once

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

void initKeyboard();

void registerKeyboard(KeyboardBuffer* buffer);
