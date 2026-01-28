#include <display.h>
#include <serial.h>
#include <syscalls.h>
#include <bootloader.h>
#include <allocator.h>
#include <cpu.h>
#include <mem.h>

uint32_t* backup = 0;
bool displayObtained = false;

void initDisplay()
{
    serialPrint("Setting up display");
    registerSyscall(23, obtainDisplay);
    registerSyscall(24, releaseDisplay);
    serialPrint("Allocating backup display buffer");
    backup = allocate(information.width * information.height * sizeof(uint32_t));
    serialPrint("Set up display");
}

void obtainDisplay(Display* display)
{
    lock(&displayObtained);
    copyMemory32(information.framebuffer, backup, information.width * information.height);
    display->buffer = information.framebuffer;
    display->width = information.width;
    display->height = information.height;
}

void releaseDisplay()
{
    copyMemory32(backup, information.framebuffer, information.width * information.height);
    unlock(&displayObtained);
}
