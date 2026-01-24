#include <scheduler.h>
#include <serial.h>
#include <idt.h>
#include <pic.h>
#include <syscalls.h>
#include <allocator.h>
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
    uint8_t stack[0x100000];
    uint64_t sp;
} Thread;

bool pitWait = false;
uint32_t ticksPerMillisecond = 0;
uint64_t ticks = 0;
Thread* threads = 0;
Thread* currentThread = 0;

__attribute__((naked)) void skipThread()
{
    pushRegisters();
    pushSseRegisters();
    __asm__ volatile ("movq %cr3, %rax; pushq %rax");
    __asm__ volatile ("movq %%rsp, %0" : "=g"(currentThread->sp));
    __asm__ volatile ("movq %1, %0" : "=g"(currentThread) : "g"(currentThread->next));
    __asm__ volatile ("jmp nextThread");
}

__attribute__((naked)) void pitWaiter()
{
    __asm__ volatile ("pushw %ax");
    __asm__ volatile ("movb $0, pitWait(%rip)");
    __asm__ volatile ("movb $0x20, %al; outb %al, $0x20");
    __asm__ volatile ("popw %ax");
    __asm__ volatile ("iretq");
}

__attribute__((naked)) void updateScheduler()
{
    pushRegisters();
    pushSseRegisters();
    __asm__ volatile ("movq %cr3, %rax; pushq %rax");
    __asm__ volatile ("movl $0xfee000B0, %eax; movl $0, (%eax)");
    __asm__ volatile ("incq ticks(%rip)");
    __asm__ volatile ("movq %%rsp, %0" : "=g"(currentThread->sp));
    __asm__ volatile ("tryNext:");
    __asm__ volatile ("movq %1, %0" : "=g"(currentThread) : "g"(currentThread->next));
    __asm__ volatile ("nextThread:");
    __asm__ volatile ("testq %0, %0; jnz tryNext" : : "g"(currentThread->waiting));
    __asm__ volatile ("movq %0, %%rsp" : : "g"(currentThread->sp));
    __asm__ volatile ("popq %rax; movq %rax, %cr3");
    popSseRegisters();
    popRegisters();
    __asm__ volatile ("iretq");
}

void initScheduler()
{
    serialPrint("Setting up scheduler");
    registerSyscall(16, getMilliseconds);
    registerSyscall(17, createThread);
    registerSyscall(18, waitForThread);
    registerSyscall(19, destroyThread);
    registerSyscall(20, exitThread);
    installIsr(0x67, skipThread);
    serialPrint("Creating main thread");
    threads = allocate(sizeof(Thread));
    threads->next = threads;
    threads->prev = threads;
    threads->id = 0;
    threads->waiting = 0;
    __asm__ volatile ("movq %%rsp, %0" : "=g"(threads->sp));
    currentThread = threads;
    serialPrint("Setting up PIT timer");
    installIrq(0, pitWaiter);
    unmaskPic(0);
    serialPrint("Setting timer divisor");
    *(uint32_t*)0xfee003E0 = 0x3;
    serialPrint("Configuring PIT timer");
    outb(0x43, 0b00110100);
    uint16_t divisor = 1193182 / 1000;
    outb(0x40, divisor);
    outb(0x40, divisor >> 8);
    pitWait = true;
    while (pitWait);
    *(uint32_t*)0xfee00380 = __UINT32_MAX__;
    pitWait = true;
    while (pitWait);
    ticksPerMillisecond = __UINT32_MAX__ - *(uint32_t*)0xfee00390;
    serialPrint("Stopping PIT timer");
    maskPic(0);
    serialPrint("Configuring LAPIC timer");
    installIsr(32, updateScheduler);
    serialPrint("Setting initial timer count");
    *(uint32_t*)0xfee00380 = ticksPerMillisecond;
    serialPrint("Setting interrupt vector");
    *(uint32_t*)0xfee00320 = 0x20020;
    serialPrint("Set up scheduler");
}

uint64_t getMilliseconds()
{
    return ticks;
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
    pushSseRegisters();
    __asm__ volatile ("movq %%cr3, %%rax; pushq %%rax" : : : "%rax");
    __asm__ volatile ("movq %%rsp, %0" : "=g"(currentThread->sp));
    __asm__ volatile ("movq %1, %0" : "=g"(currentThread) : "g"(currentThread->next));
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
    Thread* next = currentThread->next;
    ((Thread*)currentThread->prev)->next = next;
    next->prev = currentThread->prev;
    unallocate(currentThread);
    currentThread = next;
    __asm__ volatile ("jmp nextThread");
}
