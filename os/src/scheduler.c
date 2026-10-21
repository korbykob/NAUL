#include <scheduler.h>
#include <terminal.h>
#include <syscalls.h>
#include <idt.h>
#include <allocator.h>
#include <hpet.h>
#include <calls.h>
#include <cpu.h>

#define SCHEDULER_INTERRUPT 32
#define STACK_SIZE 0x100000
#define LAPIC_DIVISOR_REGISTER (APIC_BASE_ADDRESS + 0x3E0)
#define LAPIC_RELOAD_COUNT (APIC_BASE_ADDRESS + 0x380)
#define LAPIC_COUNTER (APIC_BASE_ADDRESS + 0x390)
#define LAPIC_CONFIG_REGISTER (APIC_BASE_ADDRESS + 0x320)
#define LAPIC_PERIODIC_MODE 0x20000

typedef struct
{
    uint64_t ip;
    uint64_t cs;
    uint64_t flags;
    uint64_t sp;
    uint64_t ss;
} __attribute__((packed)) InterruptFrame;
typedef struct
{
    List header;
    uint64_t id;
    Thread* thread;
} WaitingEntry;

Thread threads = { { &threads, &threads }, 0, 0, 0, 0, 0, 0, 0 };
Thread* currentThread = &threads;
List deadThreads = { &deadThreads, &deadThreads };
Thread* cleanerThread = 0;
List waitingList = { &waitingList, &waitingList };
bool waitingLock = false;

__attribute__((naked)) void nextThread()
{
    __asm__ volatile ("movq %1, %0" : "=m"(currentThread) : "r"(currentThread->header.next));
    __asm__ volatile ("testb %0, %0; jnz nextThread" : : "r"(currentThread->waiting));
    __asm__ volatile ("movq %0, %%rsp" : : "g"(currentThread->sp));
    popCr3();
    popSimdRegisters();
    popRegisters();
    __asm__ volatile ("iretq");
}

__attribute__((naked)) void skipThread()
{
    pushRegisters();
    pushSimdRegisters();
    pushCr3();
    __asm__ volatile ("movq %%rsp, %0" : "=g"(currentThread->sp));
    __asm__ volatile ("jmp nextThread");
}

__attribute__((naked)) void updateScheduler()
{
    pushRegisters();
    pushSimdRegisters();
    pushCr3();
    __asm__ volatile ("movq %%rsp, %0" : "=g"(currentThread->sp));
    __asm__ volatile ("movl $0, %0" : "=m"(*(uint32_t*)LAPIC_EOI_REGISTER));
    __asm__ volatile ("jmp nextThread");
}

void cleaner()
{
    cleanerThread = currentThread;
    while (true)
    {
        __asm__ volatile ("cli");
        while (deadThreads.next != &deadThreads)
        {
            Thread* thread = deadThreads.next;
            unlinkList(&thread->header);
            unallocate(thread->stack);
            unallocate(thread);
        }
        currentThread->waiting = true;
        __asm__ volatile ("sti");
        yieldThread();
    }
}

void initScheduler()
{
    log("Setting up scheduler");
    registerSyscall(CREATE_THREAD, createThread);
    registerSyscall(WAIT_FOR_THREAD, waitForThread);
    registerSyscall(DESTROY_THREAD, destroyThread);
    registerSyscall(EXIT_THREAD, exitThread);
    installIsr(0x67, IDT_INTERRUPT_GATE, skipThread);
    log("Configuring timer");
    installIsr(SCHEDULER_INTERRUPT, IDT_INTERRUPT_GATE, updateScheduler);
    log("Setting timer divisor");
    *(uint32_t*)LAPIC_DIVISOR_REGISTER = 3;
    log("Calibrating timer");
    *(uint32_t*)LAPIC_RELOAD_COUNT = __UINT32_MAX__;
    uint64_t start = getFemtoseconds();
    while (getFemtoseconds() - start < FEMTOSECONDS_PER_MILLISECOND);
    uint32_t ticks = __UINT32_MAX__ - *(uint32_t*)LAPIC_COUNTER;
    log("Setting initial timer count");
    *(uint32_t*)LAPIC_RELOAD_COUNT = ticks;
    log("Setting interrupt vector");
    *(uint32_t*)LAPIC_CONFIG_REGISTER = SCHEDULER_INTERRUPT | LAPIC_PERIODIC_MODE;
    log("Creating cleaner thread");
    createThread(cleaner);
    while (cleanerThread == 0)
    {
        yieldThread();
    }
    log("Set up scheduler");
}

uint64_t createThread(void (*function)())
{
    uint64_t flags = 0;
    uint64_t cs = 0;
    uint64_t ss = 0;
    __asm__ volatile ("pushfq; pop %0; movq %%cs, %1; movq %%ss, %2" : "=g"(flags), "=r"(cs), "=r"(ss));
    Thread* thread = allocate(sizeof(Thread));
    thread->header.next = currentThread;
    thread->waiting = false;
    thread->symbols = currentThread->symbols;
    thread->symbolCount = currentThread->symbolCount;
    thread->ttyId = currentThread->ttyId;
    thread->stack = allocateAligned(STACK_SIZE, 16);
    thread->sp = (uint64_t)thread->stack + STACK_SIZE - sizeof(InterruptFrame) - sizeof(void (**)());
    InterruptFrame* frame = (InterruptFrame*)thread->sp;
    frame->ip = (uint64_t)function;
    frame->cs = cs;
    frame->flags = flags;
    frame->sp = thread->sp + sizeof(InterruptFrame);
    frame->ss = ss;
    *(void (**)())(thread->sp + sizeof(InterruptFrame)) = exitThread;
    __asm__ volatile ("cli");
    __asm__ volatile ("movq %%rsp, %0" : "=g"(currentThread->sp));
    currentThread = thread;
    __asm__ volatile ("movq %0, %%rsp" : : "g"(currentThread->sp));
    __asm__ volatile ("pushq %rax; pushq %rbx; pushq %rcx; pushq %rdx; pushq %rsi; pushq %rdi; pushq $0; pushq %rsp; pushq %r8; pushq %r9; pushq %r10; pushq %r11; pushq %r12; pushq %r13; pushq %r14; pushq %r15");
    pushSimdRegisters();
    pushCr3();
    __asm__ volatile ("movq %%rsp, %0" : "=g"(currentThread->sp));
    __asm__ volatile ("movq %1, %0" : "=m"(currentThread) : "r"(currentThread->header.next));
    __asm__ volatile ("movq %0, %%rsp" : : "g"(currentThread->sp));
    uint64_t id = 0;
    Thread* current = &threads;
    while (true)
    {
        if (current->id == id)
        {
            id++;
            current = &threads;
        }
        else
        {
            current = current->header.next;
            if (current == &threads)
            {
                break;
            }
        }
    }
    thread->id = id;
    linkList(&threads.header, &thread->header);
    __asm__ volatile ("sti");
    return id;
}

void waitForThread(uint64_t id)
{
    WaitingEntry* entry = allocate(sizeof(WaitingEntry));
    entry->id = id;
    entry->thread = currentThread;
    lock(&waitingLock);
    linkList(&waitingList, &entry->header);
    unlock(&waitingLock);
    __asm__ volatile ("cli");
    currentThread->waiting = true;
    __asm__ volatile ("sti");
    yieldThread();
}

void destroyThread(uint64_t id)
{
    lock(&waitingLock);
    WaitingEntry* entry = (WaitingEntry*)&waitingList;
    while (iterateList(&waitingList, (List**)&entry))
    {
        if (entry->id == id)
        {
            __asm__ volatile ("cli");
            entry->thread->waiting = false;
            __asm__ volatile ("sti");
            WaitingEntry* prev = entry->header.prev;
            unlinkList(&entry->header);
            unallocate(entry);
            entry = prev;
        }
    }
    unlock(&waitingLock);
    __asm__ volatile ("cli");
    Thread* current = &threads;
    while (current->id != id)
    {
        current = current->header.next;
    }
    unlinkList(&current->header);
    __asm__ volatile ("sti");
    unallocate(current->stack);
    unallocate(current);
}

void exitThread()
{
    lock(&waitingLock);
    WaitingEntry* entry = (WaitingEntry*)&waitingList;
    while (iterateList(&waitingList, (List**)&entry))
    {
        if (entry->id == currentThread->id)
        {
            __asm__ volatile ("cli");
            entry->thread->waiting = false;
            __asm__ volatile ("sti");
            WaitingEntry* prev = entry->header.prev;
            unlinkList(&entry->header);
            unallocate(entry);
            entry = prev;
        }
    }
    unlock(&waitingLock);
    __asm__ volatile ("cli");
    Thread* prev = currentThread->header.prev;
    unlinkList(&currentThread->header);
    linkList(&deadThreads, &currentThread->header);
    cleanerThread->waiting = false;
    currentThread = prev;
    __asm__ volatile ("jmp nextThread");
}
