#include <hpet.h>
#include <serial.h>
#include <bootloader.h>
#include <syscalls.h>
#include <calls.h>

uint64_t femtosecondsPerTick = 0;

void initHpet()
{
    serialPrint("Setting up HPET");
    registerSyscall(GET_FEMTOSECONDS, getFemtoseconds);
    serialPrint("Getting femtoseconds per tick");
    femtosecondsPerTick = (*(uint64_t*)information.hpetAddress >> 32) & 0xFFFFFFFF;
    serialPrint("Enabling timer");
    *(uint32_t*)(information.hpetAddress + 0x10) |= 0b1;
    serialPrint("Set up HPET");
}

uint64_t getFemtoseconds()
{
    return *(uint64_t*)(information.hpetAddress + 0xF0) * femtosecondsPerTick;
}