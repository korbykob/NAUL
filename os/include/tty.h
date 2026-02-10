#pragma once

#include <definitions.h>

#define TTY_CLEAR '\xff'
#define TTY_DEFAULT "\xfe\x01"
#define TTY_WHITE "\xfe\x02"
#define TTY_BLUE "\xfe\x03"
#define TTY_GREEN "\xfe\x04"

typedef struct
{
    uint64_t readCursor;
    char* readBuffer;
    uint64_t readLength;
    uint16_t writeHead;
    uint16_t writeTail;
    char writeBuffer[65536];
} TtyBuffer;

void initTty();

void registerTty(TtyBuffer* buffer);

void unregisterTty(TtyBuffer* buffer);

void put(char character);

void write(const char* message);

void read(char* buffer, uint64_t length);
