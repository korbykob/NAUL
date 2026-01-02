#include <scheduler.h>
#include <serial.h>
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
    uint8_t stack[0x100000];
    uint64_t sp;
} Thread;

Thread* threads = 0;
Thread* currentThread = 0;

void initScheduler()
{
    serialPrint("Setting up scheduler");
    threads = allocate(sizeof(Thread));
    threads->next = threads;
    threads->prev = threads;
    threads->id = 0;
    __asm__ volatile ("movq %%rsp, %0" : "=g"(threads->sp));
    currentThread = threads;
    serialPrint("Set up scheduler");
}

__attribute__((naked)) void updateScheduler()
{
    pushRegisters();
    __asm__ volatile ("movb $0x20, %al; outb %al, $0x20");
    __asm__ volatile ("movq %%rsp, %0" : "=g"(currentThread->sp));
    __asm__ volatile ("movq %1, %0" : "=g"(currentThread) : "g"(currentThread->next));
    __asm__ volatile ("nextThread:");
    __asm__ volatile ("movq %0, %%rsp" : : "g"(currentThread->sp));
    popRegisters();
    __asm__ volatile ("iretq");
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
    unallocate(current);
    __asm__ volatile ("sti");
}

void exitThread()
{
    __asm__ volatile ("cli");
    Thread* next = currentThread->next;
    ((Thread*)currentThread->prev)->next = next;
    unallocate(currentThread);
    currentThread = next;
    __asm__ volatile ("jmp nextThread");
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
    pushRegisters();
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
