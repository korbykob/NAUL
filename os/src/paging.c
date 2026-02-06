#include <paging.h>
#include <serial.h>
#include <allocator.h>
#include <processes.h>
#include <mem.h>

#define PAGE_ENTRY_SIZE 0x1000
#define PAGE_ALIGNMENT 0x1000
#define PAGE_ENTRY_COUNT 512
#define PAGE_PRESENT 0b11
#define PAGE_SIZED 0b10000011
#define PAGE_ADDRESS_MASK ~0xFFFUL

uint64_t mainPdpt = 0;

void initPaging()
{
    serialPrint("Setting up paging");
    uint64_t* uefiPml4t = 0;
    __asm__ volatile ("mov %%cr3, %0" : "=r"(uefiPml4t));
    serialPrint("Storing paging");
    mainPdpt = uefiPml4t[0];
    serialPrint("Set up paging");
}

uint64_t createTable(void* start, uint64_t pages)
{
    uint64_t* pml4t = (uint64_t*)allocateAligned(((pages - 1) / PAGE_ENTRY_COUNT + 3) * PAGE_ENTRY_SIZE, PAGE_ALIGNMENT);
    setMemory64(pml4t, 0, PAGE_ENTRY_COUNT);
    pml4t[0] = mainPdpt;
    uint64_t* pdpt = pml4t + PAGE_ENTRY_COUNT;
    setMemory64(pdpt, 0, PAGE_ENTRY_COUNT);
    uint64_t address = (uint64_t)start;
    uint16_t table = 0;
    while (pages != 0)
    {
        uint64_t* pdt = pdpt + PAGE_ENTRY_COUNT + (table * PAGE_ENTRY_COUNT);
        setMemory64(pdt, 0, PAGE_ENTRY_COUNT);
        uint16_t count = min(pages, PAGE_ENTRY_COUNT);
        for (uint16_t i = 0; i < count; i++)
        {
            pdt[i] = address | PAGE_SIZED;
            address += PAGE_SIZE;
            pages--;
        }
        pdpt[table] = (uint64_t)pdt | PAGE_PRESENT;
        table++;
    }
    pml4t[1] = (uint64_t)pdpt | PAGE_PRESENT;
    return (uint64_t)pml4t;
}

void* getAddress(void* address)
{
    if ((uint64_t)address >= PROCESS_ADDRESS)
    {
        uint64_t* pml4t = 0;
        __asm__ volatile ("mov %%cr3, %0" : "=r"(pml4t));
        return (void*)((*(uint64_t*)(*(uint64_t*)(pml4t[1] & PAGE_ADDRESS_MASK) & PAGE_ADDRESS_MASK) & PAGE_ADDRESS_MASK) + (uint64_t)address - PROCESS_ADDRESS);
    }
    return address;
}
