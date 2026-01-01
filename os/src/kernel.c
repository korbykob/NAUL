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
#include <graphics.h>
#include <str.h>

Info* info;
uint32_t* backBuffer = 0;
uint64_t lastUpdate = 0;

void panic(uint8_t code)
{
    __asm__ volatile ("cli");
    char exceptionString[10];
    toString(exceptionString, code);
    write("\n\nCPU exception occured: ");
    write(exceptionString);
    blit(backBuffer, info->framebuffer, info->width, info->height);
    __asm__ volatile ("hlt");
}

void updateScreen()
{
    while (true)
    {
        if (getFemtoseconds() - lastUpdate >= 1000000000000000 / 60)
        {
            lastUpdate += 1000000000000000 / 60;
            blit(backBuffer, info->framebuffer, info->width, info->height);
        }
    }
}

void initVideo()
{
    serialPrint("Setting up video");
    backBuffer = allocate(info->width * info->height * sizeof(uint32_t));
    serialPrint("Clearing out back buffer");
    for (uint64_t i = 0; i < info->width * info->height; i++)
    {
        backBuffer[i] = 0;
    }
    serialPrint("Setting up screen thread");
    createThread(updateScreen);
    serialPrint("Set up video");
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
    write("Welcome to NAUL (Not Another Unix Like)!\n\n");
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
    initVideo();
    initFilesystem(info->fileData, info->fileCount);
    initTerminal(backBuffer, info->width, info->height, getFile("/font.psf", 0));
    serialPrint("Yo puter ready B)");
    welcome();
}
