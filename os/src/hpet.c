#include <hpet.h>
#include <serial.h>
#include <idt.h>
#include <pic.h>
#include <scheduler.h>

uint64_t address = 0;
uint64_t femtosecondsPerTick = 0;

void initHpet(uint64_t hpetAddress)
{
    serialPrint("Setting up HPET");
    address = hpetAddress;
    serialPrint("Installing scheduler IRQ");
    installIrq(0, updateScheduler);
    serialPrint("Enabling legacy replacement mode");
    *(uint32_t*)(address + 0x10) |= 0b11;
    serialPrint("Enabling periodic mode and interrupts");
    *(uint32_t*)(address + 0x100) |= 0b1100;
    serialPrint("Getting femtoseconds per tick");
    femtosecondsPerTick = (*(uint64_t*)address >> 32) & 0xFFFFFFFF;
    serialPrint("Setting timer tick frequency");
    *(uint64_t*)(address + 0x108) = (1000000000000000 / femtosecondsPerTick) / 10000;
    serialPrint("Resetting main counter");
    *(uint64_t*)(address + 0xF0) = 0;
    serialPrint("Unmasking HPET interrupt");
    unmaskPic(0);
    serialPrint("Set up HPET");
}

uint64_t getFemtoseconds()
{
    return *(uint64_t*)(address + 0xF0) * femtosecondsPerTick;
}
