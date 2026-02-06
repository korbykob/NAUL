#include <scheduler.h>
#include <serial.h>
#include <syscalls.h>
#include <idt.h>
#include <allocator.h>
#include <hpet.h>
#include <calls.h>
#include <cpu.h>

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
    void* next;
    void* prev;
    uint64_t id;
    uint64_t waiting;
    uint64_t sp;
    uint8_t stack[0x100000];
} Thread;

Thread* threads = 0;
Thread* currentThread = 0;

__attribute__((naked)) void nextThread()
{
    __asm__ volatile ("tryNext:");
    __asm__ volatile ("movq %1, %0" : "=m"(currentThread) : "r"(currentThread->next));
    __asm__ volatile ("testq %0, %0; jnz tryNext" : : "r"(currentThread->waiting));
    __asm__ volatile ("movq %0, %%rsp" : : "g"(currentThread->sp));
    popCr3();
    popAvxRegisters();
    popRegisters();
    __asm__ volatile ("iretq");
}

__attribute__((naked)) void skipThread()
{
    pushRegisters();
    pushAvxRegisters();
    pushCr3();
    __asm__ volatile ("movq %%rsp, %0" : "=g"(currentThread->sp));
    __asm__ volatile ("jmp nextThread");
}

__attribute__((naked)) void updateScheduler()
{
    pushRegisters();
    pushAvxRegisters();
    pushCr3();
    __asm__ volatile ("movq %%rsp, %0" : "=g"(currentThread->sp));
    __asm__ volatile ("movl $0, %0" : "=m"(*(uint32_t*)0xfee000B0));
    __asm__ volatile ("jmp nextThread");
}

void initScheduler()
{
    serialPrint("Setting up scheduler");
    registerSyscall(CREATE_THREAD, createThread);
    registerSyscall(WAIT_FOR_THREAD, waitForThread);
    registerSyscall(DESTROY_THREAD, destroyThread);
    registerSyscall(EXIT_THREAD, exitThread);
    installIsr(0x67, skipThread);
    serialPrint("Creating main thread");
    threads = allocate(sizeof(Thread));
    threads->next = threads;
    threads->prev = threads;
    threads->id = 0;
    threads->waiting = 0;
    __asm__ volatile ("movq %%rsp, %0" : "=g"(threads->sp));
    currentThread = threads;
    serialPrint("Configuring timer");
    installIsr(32, updateScheduler);
    serialPrint("Setting timer divisor");
    *(uint32_t*)0xfee003E0 = 0x3;
    serialPrint("Calibrating timer");
    *(uint32_t*)0xfee00380 = __UINT32_MAX__;
    uint64_t start = getFemtoseconds();
    while (getFemtoseconds() - start < femtosecondsPerMillisecond);
    uint32_t ticks = __UINT32_MAX__ - *(uint32_t*)0xfee00390;
    serialPrint("Setting initial timer count");
    *(uint32_t*)0xfee00380 = ticks;
    serialPrint("Setting interrupt vector");
    *(uint32_t*)0xfee00320 = 0x20020;
    serialPrint("Set up scheduler");
}

uint64_t createThread(void (*function)())
{
    uint64_t flags = 0;
    uint64_t cs = 0;
    uint64_t ss = 0;
    __asm__ volatile ("pushfq; pop %0; movq %%cs, %1; movq %%ss, %2" : "=g"(flags), "=r"(cs), "=r"(ss));
    __asm__ volatile ("cli");
    Thread* thread = allocate(sizeof(Thread));
    thread->next = currentThread;
    uint64_t id = 0;
    Thread* current = threads;
    while (true)
    {
        if (current->id == id)
        {
            id++;
            current = threads;
        }
        else
        {
            current = current->next;
            if (current == threads)
            {
                break;
            }
        }
    }
    thread->id = id;
    thread->waiting = 0;
    thread->sp = (uint64_t)&thread->stack + sizeof(thread->stack) - sizeof(InterruptFrame) - sizeof(void (**)());
    InterruptFrame* frame = (InterruptFrame*)thread->sp;
    frame->ip = (uint64_t)function;
    frame->cs = cs;
    frame->flags = flags;
    frame->sp = thread->sp + sizeof(InterruptFrame);
    frame->ss = ss;
    *(void (**)())(thread->sp + sizeof(InterruptFrame)) = exitThread;
    __asm__ volatile ("movq %%rsp, %0" : "=g"(currentThread->sp));
    currentThread = thread;
    __asm__ volatile ("movq %0, %%rsp" : : "g"(currentThread->sp));
    __asm__ volatile ("pushq %rax; pushq %rbx; pushq %rcx; pushq %rdx; pushq %rsi; pushq %rdi; pushq $0; pushq %rsp; pushq %r8; pushq %r9; pushq %r10; pushq %r11; pushq %r12; pushq %r13; pushq %r14; pushq %r15");
    pushAvxRegisters();
    pushCr3();
    __asm__ volatile ("movq %%rsp, %0" : "=g"(currentThread->sp));
    __asm__ volatile ("movq %1, %0" : "=m"(currentThread) : "r"(currentThread->next));
    __asm__ volatile ("movq %0, %%rsp" : : "g"(currentThread->sp));
    ((Thread*)threads->prev)->next = thread;
    thread->next = threads;
    thread->prev = threads->prev;
    threads->prev = thread;
    __asm__ volatile ("sti");
    return id;
}

void waitForThread(uint64_t id)
{
    currentThread->waiting = id;
    yieldThread();
}

void destroyThread(uint64_t id)
{
    __asm__ volatile ("cli");
    Thread* current = threads;
    while (current->id != id)
    {
        current = current->next;
    }
    ((Thread*)current->prev)->next = current->next;
    ((Thread*)current->next)->prev = current->prev;
    unallocate(current);
    current = threads;
    while (true)
    {
        if (current->waiting == id)
        {
            current->waiting = 0;
        }
        current = current->next;
        if (current == threads)
        {
            break;
        }
    }
    __asm__ volatile ("sti");
}

void exitThread()
{
    __asm__ volatile ("cli");
    Thread* current = threads;
    while (true)
    {
        if (current->waiting == currentThread->id)
        {
            current->waiting = 0;
        }
        current = current->next;
        if (current == threads)
        {
            break;
        }
    }
    Thread* prev = currentThread->prev;
    prev->next = currentThread->next;
    ((Thread*)currentThread->next)->prev = prev;
    unallocate(currentThread);
    currentThread = prev;
    __asm__ volatile ("jmp nextThread");
}
