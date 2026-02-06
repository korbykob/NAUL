#pragma once

#include <definitions.h>

#define yieldThread() __asm__ volatile ("int $0x67" : : : "memory")

void initScheduler();

uint64_t createThread(void (*function)());

void waitForThread(uint64_t id);

void destroyThread(uint64_t id);

void exitThread();
