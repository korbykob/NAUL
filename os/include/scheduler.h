#pragma once

#include <definitions.h>
#include <symbols.h>

#define yieldThread() __asm__ volatile ("int $0x67" : : : "memory")

typedef struct
{
    void* next;
    void* prev;
    uint64_t id;
    uint64_t waiting;
    Symbol* symbols;
    uint64_t symbolCount;
    uint8_t ttyId;
    uint8_t* stack;
    uint64_t sp;
} Thread;

extern Thread* currentThread;

void initScheduler();

uint64_t createThread(void (*function)());

void waitForThread(uint64_t id);

void destroyThread(uint64_t id);

void exitThread();
