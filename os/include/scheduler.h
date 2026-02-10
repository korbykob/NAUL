#pragma once

#include <definitions.h>

#define yieldThread() __asm__ volatile ("int $0x67" : : : "memory")

typedef struct
{
    void* next;
    void* prev;
    uint64_t id;
    uint64_t waiting;
    uint8_t ttyId;
    uint64_t sp;
    uint8_t stack[0x100000];
} Thread;

extern Thread* currentThread;

void initScheduler();

uint64_t createThread(void (*function)());

void waitForThread(uint64_t id);

void destroyThread(uint64_t id);

void exitThread();
