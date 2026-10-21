#include <hpet.h>
#include <terminal.h>
#include <bootloader.h>
#include <scheduler.h>
#include <idt.h>
#include <ioapic.h>
#include <syscalls.h>
#include <calls.h>
#include <list.h>
#include <cpu.h>

#define HPET_CAP_REGISTER 0x0
#define HPET_CONFIG_REGISTER 0x10
#define HPET_LEGACY_MODE 0b10
#define HPET_TIMER_CONFIG 0x100
#define HPET_TIMER_INTERRUPTS 0b100
#define HPET_IRQ 2
#define HPET_ENABLED 0b1
#define HPET_COUNTER 0xF0
#define HPET_TIMER_VALUE 0x108

typedef struct
{
    List header;
    uint64_t time;
    Thread* thread;
} SleepingEntry;

uint64_t femtosecondsPerTick = 0;
List sleepingList = { &sleepingList, &sleepingList };

void updateTimer()
{
    uint64_t value = *(uint64_t*)(information.hpetAddress + HPET_COUNTER);
    SleepingEntry* soonest = 0;
    SleepingEntry* entry = (SleepingEntry*)&sleepingList;
    while (iterateList(&sleepingList, (List**)&entry))
    {
        if (!soonest || (entry->time - value) < (soonest->time - value))
        {
            soonest = entry;
        }
    }
    if (soonest)
    {
        *(uint64_t*)(information.hpetAddress + HPET_TIMER_VALUE) = soonest->time;
    }
}

void hpet()
{
    bool found = false;
    uint64_t value = *(uint64_t*)(information.hpetAddress + HPET_TIMER_VALUE);
    SleepingEntry* entry = (SleepingEntry*)&sleepingList;
    while (iterateList(&sleepingList, (List**)&entry))
    {
        if (entry->time == value)
        {
            found = true;
            break;
        }
    }
    if (found)
    {
        entry->thread->waiting = false;
        unlinkList(&entry->header);
        updateTimer();
        ioapicAck();
        __asm__ volatile ("sti");
        unallocate(entry);
    }
    else
    {
        ioapicAck();
    }
}

__attribute__((naked)) void hpetInterrupt()
{
    pushRegisters();
    pushSimdRegisters();
    __asm__ volatile ("cld; call hpet");
    popSimdRegisters();
    popRegisters();
    __asm__ volatile ("iretq");
}

void initHpet()
{
    log("Setting up HPET");
    registerSyscall(GET_FEMTOSECONDS, getFemtoseconds);
    registerSyscall(WAIT_FEMTOSECONDS, waitFemtoseconds);
    registerSyscall(CANCEL_WAIT, cancelWait);
    log("Getting femtoseconds per tick");
    femtosecondsPerTick = *(uint64_t*)(information.hpetAddress + HPET_CAP_REGISTER) >> 32;
    log("Enabling legacy replacement mode");
    *(uint32_t*)(information.hpetAddress + HPET_CONFIG_REGISTER) |= HPET_LEGACY_MODE;
    log("Enabling interrupts");
    *(uint32_t*)(information.hpetAddress + HPET_TIMER_CONFIG) |= HPET_TIMER_INTERRUPTS;
    installIrq(HPET_IRQ, hpetInterrupt);
    unmaskIoapic(HPET_IRQ);
    log("Enabling timer");
    *(uint64_t*)(information.hpetAddress + HPET_CONFIG_REGISTER) |= HPET_ENABLED;
    log("Set up HPET");
}

uint64_t getFemtoseconds()
{
    return *(uint64_t*)(information.hpetAddress + HPET_COUNTER) * femtosecondsPerTick;
}

void waitFemtoseconds(uint64_t femtoseconds)
{
    SleepingEntry* entry = allocate(sizeof(SleepingEntry));
    entry->time = (femtoseconds / femtosecondsPerTick) + *(uint64_t*)(information.hpetAddress + HPET_COUNTER);
    entry->thread = currentThread;
    __asm__ volatile ("cli");
    linkList(&sleepingList, &entry->header);
    currentThread->waiting = true;
    updateTimer();
    __asm__ volatile ("sti");
    yieldThread();
}

void cancelWait(uint64_t id)
{
    __asm__ volatile ("cli");
    SleepingEntry* entry = (SleepingEntry*)&sleepingList;
    while (iterateList(&sleepingList, (List**)&entry))
    {
        if (entry->thread->id == id)
        {
            break;
        }
    }
    entry->thread->waiting = false;
    unlinkList(&entry->header);
    updateTimer();
    __asm__ volatile ("sti");
    unallocate(entry);
}
