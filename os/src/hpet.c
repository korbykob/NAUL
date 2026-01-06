#include <hpet.h>
#include <bootloader.h>
#include <serial.h>
#include <idt.h>
#include <pic.h>
#include <scheduler.h>

uint64_t femtosecondsPerTick = 0;

void initHpet()
{
    serialPrint("Setting up HPET");
    installIrq(0, updateScheduler);
    serialPrint("Enabling legacy replacement mode");
    *(uint32_t*)(information.hpetAddress + 0x10) |= 0b11;
    serialPrint("Enabling periodic mode and interrupts");
    *(uint32_t*)(information.hpetAddress + 0x100) |= 0b1100;
    serialPrint("Getting femtoseconds per tick");
    femtosecondsPerTick = (*(uint64_t*)information.hpetAddress >> 32) & 0xFFFFFFFF;
    serialPrint("Setting timer tick frequency");
    *(uint64_t*)(information.hpetAddress + 0x108) = (femtosecondsPerSecond / femtosecondsPerTick) / 10000;
    serialPrint("Resetting main counter");
    *(uint64_t*)(information.hpetAddress + 0xF0) = 0;
    serialPrint("Unmasking HPET interrupt");
    unmaskPic(0);
    serialPrint("Set up HPET");
}

uint64_t getFemtoseconds()
{
    return *(uint64_t*)(information.hpetAddress + 0xF0) * femtosecondsPerTick;
}
