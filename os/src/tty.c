#include <tty.h>
#include <serial.h>
#include <calls.h>
#include <syscalls.h>
#include <paging.h>
#include <scheduler.h>
#include <cpu.h>
#include <str.h>
#include <mem.h>

struct
{
    bool used;
    uint8_t oldId;
    TtyBuffer* buffer;
} ttyBuffers[256];
bool ttyLock = false;

void initTty()
{
    serialPrint("Setting up TTY");
    registerSyscall(REGISTER_TTY, registerTty);
    registerSyscall(UNREGISTER_TTY, unregisterTty);
    registerSyscall(PUT, put);
    registerSyscall(WRITE, write);
    registerSyscall(READ, read);
    serialPrint("Clearing out TTY buffers");
    for (uint16_t i = 0; i < 256; i++)
    {
        ttyBuffers[i].used = false;
    }
    serialPrint("Set up TTY");
}

void registerTty(TtyBuffer* buffer)
{
    lock(&ttyLock);
    uint8_t id = 0;
    while (ttyBuffers[id].used)
    {
        id++;
    }
    ttyBuffers[id].used = true;
    ttyBuffers[id].oldId = currentThread->ttyId;
    ttyBuffers[id].buffer = getAddress(buffer);
    currentThread->ttyId = id;
    unlock(&ttyLock);
}

void unregisterTty(TtyBuffer* buffer)
{
    lock(&ttyLock);
    TtyBuffer* address = getAddress(buffer);
    uint8_t id = 0;
    while (ttyBuffers[id].buffer != address)
    {
        id++;
    }
    currentThread->ttyId = ttyBuffers[id].oldId;
    ttyBuffers[id].used = false;
    unlock(&ttyLock);
}

void put(char character)
{
    lock(&ttyLock);
    TtyBuffer* tty = ttyBuffers[currentThread->ttyId].buffer;
    tty->writeBuffer[tty->writeHead] = character;
    tty->writeHead++;
    unlock(&ttyLock);
}

void write(const char* message)
{
    lock(&ttyLock);
    TtyBuffer* tty = ttyBuffers[currentThread->ttyId].buffer;
    uint64_t length = stringLength(message);
    copyMemory8((uint8_t*)message, (uint8_t*)&tty->writeBuffer[tty->writeHead], length);
    tty->writeHead += length;
    unlock(&ttyLock);
}

void read(char* buffer, uint64_t length)
{
    lock(&ttyLock);
    TtyBuffer* tty = ttyBuffers[currentThread->ttyId].buffer;
    tty->readCursor = 0;
    tty->readBuffer = getAddress(buffer);
    tty->readLength = length;
    unlock(&ttyLock);
    while (tty->readBuffer)
    {
        yieldThread();
    }
}
