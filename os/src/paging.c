#include <paging.h>
#include <serial.h>
#include <allocator.h>
#include <mem.h>

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
    uint64_t* pml4t = (uint64_t*)allocateAligned(0x1000 + 0x1000 + ((pages - 1) / 512 + 1) * 0x1000, 0x1000);
    setMemory64(pml4t, 0, 512);
    pml4t[0] = mainPdpt;
    uint64_t* pdpt = pml4t + 512;
    setMemory64(pdpt, 0, 512);
    uint64_t address = (uint64_t)start;
    uint16_t table = 0;
    while (pages != 0)
    {
        uint64_t* pdt = pdpt + 512 + (table * 512);
        setMemory64(pdt, 0, 512);
        uint16_t count = min(pages, 512);
        for (uint16_t i = 0; i < count; i++)
        {
            pdt[i] = address | 0b10000011;
            address += 0x200000;
            pages--;
        }
        pdpt[table] = (uint64_t)pdt | 0b11;
        table++;
    }
    pml4t[1] = (uint64_t)pdpt | 0b11;
    return (uint64_t)pml4t;
}

void* getAddress(void* address)
{
    if ((uint64_t)address >= 0x8000000000)
    {
        uint64_t* pml4t = 0;
        __asm__ volatile ("mov %%cr3, %0" : "=r"(pml4t));
        return (void*)((*(uint64_t*)(*(uint64_t*)(pml4t[1] & ~0xFFFUL) & ~0xFFFUL) & ~0xFFFUL) + (uint64_t)address - 0x8000000000);
    }
    return address;
}
