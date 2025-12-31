#include <paging.h>
#include <serial.h>
#include <allocator.h>

uint64_t mainPdpt = 0;

uint64_t createPDPT(void* start, uint64_t pages)
{
    uint64_t* PDPT = (uint64_t*)allocateAligned(0x1000 + ((pages - 1) / 512 + 1) * 0x1000, 0x1000);
    for (uint16_t i = 0; i < 512; i++)
    {
        PDPT[i] = 0;
    }
    uint64_t address = (uint64_t)start;
    uint16_t table = 0;
    while (pages != 0)
    {
        uint64_t* PDT = PDPT + 512 + (table * 512);
        for (uint16_t i = 0; i < 512; i++)
        {
            PDT[i] = 0;
        }
        uint16_t count = min(pages, 512);
        for (uint16_t i = 0; i < count; i++)
        {
            PDT[i] = address | 0b10000011;
            address += 0x200000;
            pages--;
        }
        PDPT[table] = (uint64_t)PDT | 0b11;
        table++;
    }
    return (uint64_t)PDPT | 0b11;
}

void initPaging()
{
    serialPrint("Setting up level 4 paging");
    uint64_t* PML4T = (uint64_t*)allocateAligned(0x1000, 0x1000);
    serialPrint("Clearing out table");
    for (uint16_t i = 0; i < 512; i++)
    {
        PML4T[i] = 0;
    }
    serialPrint("Allocating first entry");
    mainPdpt = createPDPT(0, 0x40000);
    PML4T[0] = mainPdpt;
    serialPrint("Applying paging");
    __asm__ volatile ("mov %0, %%cr3" : : "r"(PML4T));
    serialPrint("Set up paging");
}

uint64_t createTable(void* start, uint64_t pages)
{
    uint64_t* PML4T = (uint64_t*)allocateAligned(0x1000, 0x1000);
    for (uint16_t i = 0; i < 512; i++)
    {
        PML4T[i] = 0;
    }
    PML4T[0] = mainPdpt;
    PML4T[1] = createPDPT(start, pages);
    return (uint64_t)PML4T;
}

void* getAddress(void* address)
{
    if ((uint64_t)address >= 0x8000000000)
    {
        uint64_t* PML4T = 0;
        __asm__ volatile ("mov %%cr3, %0" : "=r"(PML4T));
        return (void*)((*(uint64_t*)(*(uint64_t*)(PML4T[1] & ~0xFFFUL) & ~0xFFFUL) & ~0xFFFUL) + (uint64_t)address - 0x8000000000);
    }
    return address;
}
