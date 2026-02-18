#include <processes.h>
#include <serial.h>
#include <syscalls.h>
#include <calls.h>
#include <filesystem.h>
#include <allocator.h>
#include <paging.h>
#include <scheduler.h>
#include <symbols.h>
#include <mem.h>
#include <str.h>

void initProcesses()
{
    serialPrint("Setting up processes");
    registerSyscall(EXECUTE, execute);
    registerSyscall(QUIT, quit);
    serialPrint("Set up processes");
}

uint64_t execute(const char* filename)
{
    uint64_t size = 0;
    uint8_t* data = getFile(filename, &size);
    size -= sizeof(uint64_t);
    uint8_t* program = allocateAligned(size, PAGE_SIZE);
    copyMemory8(data + sizeof(uint64_t), program, size);
    uint64_t nameLength = stringLength(filename);
    char* symbolsFile = allocate(nameLength + 1);
    copyString(filename, symbolsFile);
    copyString("sym", symbolsFile + nameLength - 3);
    bool hasSymbols = checkFile(symbolsFile);
    Symbol* oldSymbols = currentThread->symbols;
    uint64_t oldCount = currentThread->symbolCount;
    if (hasSymbols)
    {
        currentThread->symbols = parseSymbols(symbolsFile, &currentThread->symbolCount);
    }
    else
    {
        currentThread->symbols = 0;
        currentThread->symbolCount = 0;
    }
    unallocate(symbolsFile);
    uint64_t table = 0;
    __asm__ volatile ("mov %%cr3, %0" : "=r"(table));
    __asm__ volatile ("mov %0, %%cr3" : : "r"(createTable(program, (size - 1) / PAGE_SIZE + 1)));
    uint64_t thread = createThread((void (*)())(PROCESS_ADDRESS + *(uint64_t*)data - sizeof(uint64_t)));
    __asm__ volatile ("mov %0, %%cr3" : : "r"(table));
    currentThread->symbols = oldSymbols;
    currentThread->symbolCount = oldCount;
    return thread;
}

void quit()
{
    unallocate(getAddress((void*)PROCESS_ADDRESS));
    uint64_t table = 0;
    __asm__ volatile ("mov %%cr3, %0" : "=r"(table));
    unallocate((void*)table);
    if (currentThread->symbols)
    {
        unallocate(currentThread->symbols);
    }
    exitThread();
}
