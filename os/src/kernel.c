#include <kernel.h>
#include <serial.h>
#include <filesystem.h>
#include <panic.h>
#include <symbols.h>
#include <allocator.h>
#include <gdt.h>
#include <idt.h>
#include <terminal.h>
#include <paging.h>
#include <syscalls.h>
#include <pic.h>
#include <hpet.h>
#include <scheduler.h>
#include <keyboard.h>
#include <display.h>
#include <mem.h>

uint64_t execute(const char* filename)
{
    uint64_t size = 0;
    uint8_t* data = getFile(filename, &size);
    size -= 4;
    uint8_t* program = allocateAligned(size, 0x200000);
    copyMemory8(data + 4, program, size);
    uint64_t table = 0;
    __asm__ volatile ("mov %%cr3, %0" : "=r"(table));
    __asm__ volatile ("mov %0, %%cr3" : : "r"(createTable(program, (size - 1) / 0x200000 + 1)));
    uint64_t thread = createThread((void (*)())(0x8000000000 + *(uint32_t*)data - 4));
    __asm__ volatile ("mov %0, %%cr3" : : "r"(table));
    return thread;
}

void quit()
{
    unallocate(getAddress((void*)0x8000000000));
    uint64_t table = 0;
    __asm__ volatile ("mov %%cr3, %0" : "=r"(table));
    unallocate((void*)table);
    exitThread();
}

void kernel()
{
    initGdt();
    initIdt();
    initPaging();
    initFilesystem();
    initPanic();
    initSymbols();
    initSyscalls();
    registerSyscall(0, execute);
    registerSyscall(1, quit);
    initPic();
    initHpet();
    initScheduler();
    initKeyboard();
    initDisplay();
    initTerminal();
    serialPrint("Yo puter ready B)");
    write("Welcome to " TERM_WHITE "NAUL" TERM_DEFAULT " (Not A Unix Like)!\n\nStarting shell, use \"" TERM_GREEN "help" TERM_DEFAULT "\" for more information:\n");
    execute("/programs/shell.bin");
}
