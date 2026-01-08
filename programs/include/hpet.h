#pragma once

#include <definitions.h>

#define femtosecondsPerSecond 1000000000000000
#define femtosecondsPerMicrosecond 1000000000

static uint64_t getFemtoseconds()
{
    uint64_t result = 0;
    __asm__ volatile ("movq $16, %%rdi; int $0x69; movq %%rax, %0" : "=g"(result) : : "%rdi", "%rax");
    return result;
}
