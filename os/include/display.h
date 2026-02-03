#pragma once

#include <definitions.h>

typedef struct
{
    uint32_t* buffer;
    uint32_t width;
    uint32_t height;
} Display;

extern bool displayObtained;

void initDisplay();

void obtainDisplay(Display* display);

void releaseDisplay();
