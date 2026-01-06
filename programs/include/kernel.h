#pragma once

#include <definitions.h>

static uint64_t execute(const char* filename)
{
    uint64_t result = 0;
    __asm__ volatile ("movq $0, %%rdi; movq %1, %%rsi; int $0x69; movq %%rax, %0" : "=g"(result) : "g"(filename) : "%rdi", "%rsi", "%rax");
    return result;
}

static void quit()
{
    __asm__ volatile ("movq $1, %%rdi; int $0x69" : :  : "%rdi", "%rax");
}
