#pragma once

#include <definitions.h>

static void* allocate(uint64_t amount)
{
    uint64_t result = 0;
    __asm__ volatile ("movq $6, %%rdi; movq %1, %%rsi; int $0x69; movq %%rax, %0" : "=g"(result) : "g"(amount) : "%rdi", "%rsi", "%rax");
    return (void*)result;
}

static void* allocateAligned(uint64_t amount, uint64_t alignment)
{
    uint64_t result = 0;
    __asm__ volatile ("movq $7, %%rdi; movq %1, %%rsi; movq %2, %%rdx; int $0x69; movq %%rax, %0" : "=g"(result) : "g"(amount), "g"(alignment) : "%rdi", "%rsi", "%rdx", "%rax");
    return (void*)result;
}

static void unallocate(void* pointer)
{
    __asm__ volatile ("movq $8, %%rdi; movq %0, %%rsi; int $0x69" : : "g"(pointer) : "%rdi", "%rsi", "%rax");
}
