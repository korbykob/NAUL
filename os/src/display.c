#include <display.h>
#include <terminal.h>
#include <syscalls.h>
#include <bootloader.h>
#include <calls.h>
#include <cpu.h>

bool displayObtained = false;

void initDisplay()
{
    log("Setting up display");
    registerSyscall(OBTAIN_DISPLAY, obtainDisplay);
    registerSyscall(RELEASE_DISPLAY, releaseDisplay);
    log("Set up display");
}

void obtainDisplay(Display* display)
{
    lock(&displayObtained);
    display->buffer = information.framebuffer;
    display->width = information.width;
    display->height = information.height;
    display->pitch = information.pitch;
}

void releaseDisplay()
{
    redrawTerminal();
    unlock(&displayObtained);
}
