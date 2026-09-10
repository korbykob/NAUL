#pragma once

#include <serial.h>
#include <tty.h>

void initTerminal();

void startTerminal();

void terminalPut(char character);

void terminalWrite(const char* string);

void redrawTerminal();

static void log(const char* string)
{
    serialWrite("\x1b[1;34m[");
    terminalWrite(TTY_BLUE "[");
    serialWrite(__builtin_strrchr(__BASE_FILE__, '/') + 1);
    terminalWrite(__builtin_strrchr(__BASE_FILE__, '/') + 1);
    serialWrite("]:\x1b[0m ");
    terminalWrite("]:" TTY_DEFAULT " ");
    serialWrite(string);
    terminalWrite(string);
    serialPut('\n');
    terminalPut('\n');
}
