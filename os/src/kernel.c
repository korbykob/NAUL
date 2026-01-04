#include <kernel.h>
#include <serial.h>
#include <filesystem.h>
#include <allocator.h>
#include <gdt.h>
#include <idt.h>
#include <terminal.h>
#include <paging.h>
#include <syscalls.h>
#include <pic.h>
#include <scheduler.h>
#include <hpet.h>
#include <keyboard.h>
#include <str.h>

Info* info;

void panic(uint8_t code)
{
    __asm__ volatile ("cli");
    char exceptionString[10];
    toString(exceptionString, code);
    write("\n\nCPU exception occured: ");
    write(exceptionString);
    __asm__ volatile ("hlt");
}

void execute(const char* filename)
{
    uint64_t size = 0;
    uint8_t* data = getFile(filename, &size);
    size -= 4;
    uint8_t* program = allocateAligned(size, 0x200000);
    for (uint64_t i = 0; i < size; i++)
    {
        program[i] = data[i + 4];
    }
    uint64_t table = createTable(program, (size - 1) / 0x200000 + 1);
    __asm__ volatile ("mov %0, %%cr3" : : "r"(table));
    createThread((void (*)())(0x8000000000 + *(uint32_t*)data - 4));
}

void welcome()
{
    write("Welcome to NAUL (Not A Unix Like)!\n\n");
    execute("/test.bin");
}

void kernel(Info* information)
{
    info = information;
    initPaging();
    initGdt();
    initIdt();
    initSyscalls();
    initPic();
    initKeyboard();
    initScheduler();
    initHpet(info->hpetAddress);
    initFilesystem(info->fileData, info->fileCount);
    initTerminal(info->framebuffer, info->width, info->height, getFile("/font.psf", 0));
    serialPrint("Yo puter ready B)");
    welcome();
}
