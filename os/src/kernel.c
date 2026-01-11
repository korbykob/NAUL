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

typedef struct
{
    void* next;
    uint64_t address;
} FramePointer;

const Exception exceptions[32] = {
    { "Division error", false },
    { "Debug", false },
    { "Non-maskable interrupt", false },
    { "Breakpoint", false },
    { "Overflow", false },
    { "Bound range exceeded", false },
    { "Invalid opcode", false },
    { "Device not available", false },
    { "Double fault", true },
    { "Coprocessor segment overrun", false },
    { "Invalid tss", true },
    { "Segment not present", true },
    { "Stack-segment fault", true },
    { "General protection fault", true },
    { "Page fault", true },
    { "", false },
    { "x87 floating-point exception", false },
    { "Alignment check", true },
    { "Machine check", false },
    { "SIMD floating-point exception", false },
    { "Virtualization exception", false },
    { "Control protection exception", true },
    { "", false },
    { "", false },
    { "", false },
    { "", false },
    { "", false },
    { "", false },
    { "Hypervisor injection exception", false },
    { "VMM communication exception", true },
    { "Security exception", true },
    { "", false }
};

void panic(uint8_t exception, uint32_t code)
{
    __asm__ volatile ("cli");
    serialPut('\n');
    serialWrite(exceptions[exception].name);
    serialWrite(" occured");
    if (exceptions[exception].code)
    {
        serialWrite(" with error code 0x");
        char codeString[9];
        toHex(codeString, code);
        serialWrite(codeString);
    }
    serialWrite(" in:\n");
    FramePointer* frame = 0;
    __asm__ volatile ("movq %%rbp, %0" : "=g"(frame));
    while (frame)
    {
        uint64_t address = frame->address - 5;
        bool process = address >= 0x8000000000;
        if (process)
        {
            address -= 0x8000000000;
            serialWrite("Within a process");
        }
        else
        {
            address -= getOffset();
            uint64_t offset = 0;
            serialWrite(getSymbol(address, &offset));
            serialWrite("+0x");
            char offsetString[17];
            toHex(offsetString, offset);
            serialWrite(offsetString);
        }
        serialWrite(" (0x");
        char offsetString[17];
        toHex(offsetString, address);
        serialWrite(offsetString);
        serialWrite(")\n");
        frame = frame->next;
        if (process && frame->next == 0)
        {
            break;
        }
    }
    __asm__ volatile ("hlt");
}

uint64_t execute(const char* filename)
{
    uint64_t size = 0;
    uint8_t* data = getFile(filename, &size);
    size -= 4;
    uint8_t* program = allocateAligned(size, 0x200000);
    for (uint64_t i = 0; i < size; i++)
    {
        program[i] = data[i + 4];
    }
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
    unallocateTable(table);
    exitThread();
}

void kernel()
{
    __asm__ volatile ("xorq %rbp, %rbp");
    initGdt();
    initIdt();
    initPaging();
    initFilesystem();
    initSymbols();
    initSyscalls();
    registerSyscall(0, execute);
    registerSyscall(1, quit);
    initPic();
    initKeyboard();
    initScheduler();
    initHpet();
    initTerminal();
    serialPrint("Yo puter ready B)");
    write("Welcome to NAUL (Not A Unix Like)!\n\nStarting shell, use \"help\" for more information:\n");
    execute("/programs/shell.bin");
}
