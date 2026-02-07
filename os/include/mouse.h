#pragma once

#include <definitions.h>

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

void initMouse();

void registerMouse(MouseBuffer* buffer);

void unregisterMouse(MouseBuffer* buffer);
