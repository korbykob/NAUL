#include <kernel.h>
#include <filesystem.h>
#include <terminal.h>
#include <gdt.h>
#include <idt.h>
#include <pic.h>
#include <symbols.h>
#include <panic.h>
#include <paging.h>
#include <syscalls.h>
#include <hpet.h>
#include <scheduler.h>
#include <power.h>
#include <keyboard.h>
#include <mouse.h>
#include <display.h>
#include <tty.h>
#include <processes.h>
#include <ipc.h>

void kernel()
{
    initFilesystem();
    initTerminal();
    initGdt();
    initIdt();
    initPic();
    initSymbols();
    initPanic();
    initPaging();
    initSyscalls();
    initHpet();
    initScheduler();
    initPower();
    initKeyboard();
    initMouse();
    initDisplay();
    initTty();
    initProcesses();
    initIpc();
    log("Starting terminal");
    startTerminal();
    log("Yo puter ready B)");
    put(TTY_CLEAR);
    write("Welcome to " TTY_WHITE "NAUL" TTY_DEFAULT " (Not A Unix Like)!\n\nStarting shell, use \"" TTY_GREEN "help" TTY_DEFAULT "\" for more information:\n");
    while (true)
    {
        waitForThread(execute("/programs/shell/shell.nxe"));
        write(TTY_RED "\nShell process died, restarting:\n" TTY_DEFAULT);
    }
}
