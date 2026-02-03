#pragma once

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

void initMouse();

void registerMouse(MouseBuffer* buffer);

void unregisterMouse(MouseBuffer* buffer);
