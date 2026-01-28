#pragma once

#include <definitions.h>

typedef struct
{
    uint32_t* buffer;
    uint32_t width;
    uint64_t height;
} Display;

void initDisplay();

void obtainDisplay(Display* display);

void releaseDisplay();
