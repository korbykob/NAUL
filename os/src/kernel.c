#include <kernel.h>
#include <serial.h>
#include <filesystem.h>
#include <symbols.h>
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

const Exception exceptions[32] = {
    { "division error", false },
    { "debug", false },
    { "non-maskable interrupt", false },
    { "breakpoint", false },
    { "overflow", false },
    { "bound range exceeded", false },
    { "invalid opcode", false },
    { "device not available", false },
    { "double fault", true },
    { "coprocessor segment overrun", false },
    { "invalid tss", true },
    { "segment not present", true },
    { "stack-segment fault", true },
    { "general protection fault", true },
    { "page fault", true },
    { "", false },
    { "x87 floating-point exception", false },
    { "alignment check", true },
    { "machine check", false },
    { "SIMD floating-point exception", false },
    { "virtualization exception", false },
    { "control protection exception", true },
    { "", false },
    { "", false },
    { "", false },
    { "", false },
    { "", false },
    { "", false },
    { "hypervisor injection exception", false },
    { "VMM communication exception", true },
    { "security exception", true },
    { "", false }
};

void panic(uint8_t exception, uint32_t code, uint64_t address)
{
    serialWrite("\nA ");
    serialWrite(exceptions[exception].name);
    serialWrite(" occured");
    if (exceptions[exception].code)
    {
        serialWrite(" with error code 0x");
        char codeString[10];
        toHex(codeString, code);
        serialWrite(codeString);
    }
    serialWrite(" in ");
    if (address > 0x8000000000)
    {
        serialWrite("a user process");
    }
    else
    {
        uint64_t offset = 0;
        serialWrite(getSymbol(address, &offset));
        serialWrite("+0x");
        char offsetString[10];
        toHex(offsetString, offset);
        serialWrite(offsetString);
    }
    serialPut('\n');
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

void kernel()
{
    initPaging();
    initGdt();
    initFilesystem();
    initSymbols();
    initIdt();
    initSyscalls();
    initPic();
    initKeyboard();
    initScheduler();
    initHpet();
    initTerminal();
    serialPrint("Yo puter ready B)");
    welcome();
}
