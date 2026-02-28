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
#include <tty.h>
#include <terminal.h>
#include <processes.h>
#include <ipc.h>

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
    initTty();
    initTerminal();
    initProcesses();
    initIpc();
    serialPrint("Yo puter ready B)");
    write("Welcome to " TTY_WHITE "NAUL" TTY_DEFAULT " (Not A Unix Like)!\n\nStarting shell, use \"" TTY_GREEN "help" TTY_DEFAULT "\" for more information:\n");
    while (true)
    {
        waitForThread(execute("/programs/shell/shell.bin"));
        write(TTY_RED "\nShell process died, restarting:\n" TTY_DEFAULT);
    }
}
