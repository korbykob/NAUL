#include <display.h>
#include <serial.h>
#include <syscalls.h>
#include <bootloader.h>
#include <allocator.h>
#include <calls.h>
#include <terminal.h>
#include <cpu.h>
#include <mem.h>

bool displayObtained = false;

void initDisplay()
{
    serialPrint("Setting up display");
    registerSyscall(OBTAIN_DISPLAY, obtainDisplay);
    registerSyscall(RELEASE_DISPLAY, releaseDisplay);
    serialPrint("Set up display");
}

void obtainDisplay(Display* display)
{
    lock(&displayObtained);
    display->buffer = information.framebuffer;
    display->width = information.width;
    display->height = information.height;
}

void releaseDisplay()
{
    redrawTerminal();
    unlock(&displayObtained);
}
