#pragma once

#include "syscalls.h"
#include <definitions.h>
#include <calls.h>

#define TTY_CLEAR '\xff'
#define TTY_DEFAULT "\xfe\x01"
#define TTY_WHITE "\xfe\x02"
#define TTY_BLUE "\xfe\x03"
#define TTY_GREEN "\xfe\x04"
#define TTY_RED "\xfe\x05"

typedef struct
{
    uint64_t readCursor;
    char* readBuffer;
    uint64_t readLength;
    uint16_t writeHead;
    uint16_t writeTail;
    char writeBuffer[65536];
} TtyBuffer;

static inline void registerTty(TtyBuffer* buffer)
{
    SYSCALL_1(REGISTER_TTY, buffer);
}

static inline void unregisterTty(TtyBuffer* buffer)
{
    SYSCALL_1(UNREGISTER_TTY, buffer);
}

static inline void put(char character)
{
    SYSCALL_1(PUT, character);
}

static inline void write(const char* message)
{
    SYSCALL_1(WRITE, message);
}

static inline void read(char* buffer, uint64_t length)
{
    SYSCALL_2(READ, buffer, length);
}
