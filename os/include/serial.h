#pragma once

void serialPut(char character);

void serialWrite(const char* string);

static void serialPrint(const char* string)
{
    serialPut('[');
    serialWrite(__builtin_strrchr(__BASE_FILE__, '/') + 1);
    serialWrite("]: ");
    serialWrite(string);
    serialPut('\n');
}
