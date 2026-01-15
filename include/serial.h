#pragma once

#include <io.h>

static void serialPut(char character)
{
    while ((inb(0x3ED) & 0x20) == 0);
    outb(0x3E8, character);
}

static void serialWrite(const char* string)
{
    while (*string)
    {
        serialPut(*string++);
    }
}

static void serialPrint(const char* string)
{
    serialWrite("\e[1;34m[");
    serialWrite(__builtin_strrchr(__BASE_FILE__, '/') + 1);
    serialWrite("]:\e[0m ");
    serialWrite(string);
    serialPut('\n');
}
