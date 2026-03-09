#include <allocator.h>
#include <serial.h>
#include <syscalls.h>
#include <calls.h>
#include <cpu.h>

#define MEMORY_START 0x1000000

typedef struct
{
    bool present;
    uint64_t start;
    uint64_t end;
} Allocation;

Allocation* allocated = 0;
uint64_t allocations = 0;

void initAllocator(uint64_t end)
{
    serialPrint("Setting up allocator");
    registerSyscall(ALLOCATE, allocate);
    registerSyscall(ALLOCATE_ALIGNED, allocateAligned);
    registerSyscall(UNALLOCATE, unallocate);
    serialPrint("Storing allocation location");
    allocated = (Allocation*)(end - sizeof(Allocation));
    serialPrint("Set up allocator");
}

void markUnusable(uint64_t start, uint64_t end)
{
    Allocation* allocation = allocated;
    uint64_t count = 0;
    while (allocation->present && count != allocations)
    {
        count++;
        allocation--;
    }
    allocation->present = true;
    allocation->start = start;
    allocation->end = end;
    allocations++;
}

void* allocate(uint64_t amount)
{
    __asm__ volatile ("cli");
    uint64_t value = MEMORY_START;
    Allocation* allocation = allocated;
    uint64_t count = 0;
    while (count != allocations)
    {
        if (allocation->present)
        {
            count++;
            if (value + amount <= allocation->start || value >= allocation->end)
            {
                allocation--;
            }
            else
            {
                value = allocation->end;
                allocation = allocated;
                count = 0;
            }
        }
        else
        {
            allocation--;
        }
    }
    allocation = allocated;
    count = 0;
    while (allocation->present && count != allocations)
    {
        count++;
        allocation--;
    }
    allocation->present = true;
    allocation->start = value;
    allocation->end = value + amount;
    allocations++;
    __asm__ volatile ("sti");
    return (void*)value;
}

void* allocateAligned(uint64_t amount, uint64_t alignment)
{
    __asm__ volatile ("cli");
    uint64_t value = MEMORY_START + (alignment - (MEMORY_START % alignment));
    Allocation* allocation = allocated;
    uint64_t count = 0;
    while (count != allocations)
    {
        if (allocation->present)
        {
            count++;
            if (value + amount <= allocation->start || value >= allocation->end)
            {
                allocation--;
            }
            else
            {
                value = allocation->end + (alignment - (allocation->end % alignment));
                allocation = allocated;
                count = 0;
            }
        }
        else
        {
            allocation--;
        }
    }
    allocation = allocated;
    count = 0;
    while (allocation->present && count != allocations)
    {
        count++;
        allocation--;
    }
    allocation->present = true;
    allocation->start = value;
    allocation->end = value + amount;
    allocations++;
    __asm__ volatile ("sti");
    return (void*)value;
}

void unallocate(void* pointer)
{
    __asm__ volatile ("cli");
    Allocation* test = allocated;
    while (!test->present || test->start != (uint64_t)pointer)
    {
        test--;
    }
    test->present = false;
    allocations--;
    __asm__ volatile ("sti");
}
