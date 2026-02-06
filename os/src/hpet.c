#include <hpet.h>
#include <serial.h>
#include <bootloader.h>
#include <syscalls.h>
#include <calls.h>

#define HPET_CAP_REGISTER 0x0
#define HPET_CONFIG_REGISTER 0x10
#define HPET_ENABLED 0b1
#define HPET_COUNTER 0xF0

uint64_t femtosecondsPerTick = 0;

void initHpet()
{
    serialPrint("Setting up HPET");
    registerSyscall(GET_FEMTOSECONDS, getFemtoseconds);
    serialPrint("Getting femtoseconds per tick");
    femtosecondsPerTick = *(uint64_t*)(information.hpetAddress + HPET_CAP_REGISTER) >> 32;
    serialPrint("Enabling timer");
    *(uint64_t*)(information.hpetAddress + HPET_CONFIG_REGISTER) |= HPET_ENABLED;
    serialPrint("Set up HPET");
}

uint64_t getFemtoseconds()
{
    return *(uint64_t*)(information.hpetAddress + HPET_COUNTER) * femtosecondsPerTick;
}
