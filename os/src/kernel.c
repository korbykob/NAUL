#include <kernel.h>
#include <serial.h>
#include <gdt.h>
#include <idt.h>
#include <filesystem.h>
#include <symbols.h>
#include <panic.h>
#include <paging.h>
#include <syscalls.h>
#include <pic.h>
#include <hpet.h>
#include <scheduler.h>
#include <keyboard.h>
#include <mouse.h>
#include <display.h>
#include <terminal.h>
#include <processes.h>

void kernel()
{
    initGdt();
    initIdt();
    initFilesystem();
    initSymbols();
    initPanic();
    initPaging();
    initSyscalls();
    initPic();
    initHpet();
    initScheduler();
    initKeyboard();
    initMouse();
    initDisplay();
    initTerminal();
    initProcesses();
    serialPrint("Yo puter ready B)");
    write("Welcome to " TERM_WHITE "NAUL" TERM_DEFAULT " (Not A Unix Like)!\n\nStarting shell, use \"" TERM_GREEN "help" TERM_DEFAULT "\" for more information:\n");
    execute("/programs/shell.bin");
    exitThread();
}
