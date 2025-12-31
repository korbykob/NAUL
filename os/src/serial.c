#include <serial.h>
#include <io.h>

void serialPut(char character)
{
    while ((inb(0x3ED) & 0x20) == 0);
    outb(0x3E8, character);
}

void serialWrite(const char* string)
{
    while (*string)
    {
        serialPut(*string++);
    }
}
