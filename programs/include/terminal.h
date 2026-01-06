#pragma once

static void put(char character)
{
    __asm__ volatile ("movq $2, %%rdi; movq %0, %%rsi; int $0x69" : : "g"(character) : "%rdi", "%rsi", "%rax");
}

static void write(const char* message)
{
    __asm__ volatile ("movq $3, %%rdi; movq %0, %%rsi; int $0x69" : : "g"(message) : "%rdi", "%rsi", "%rax");
}

static void clear()
{
    __asm__ volatile ("movq $4, %%rdi; int $0x69" : :  : "%rdi", "%rax");
}

static void read(char* buffer)
{
    __asm__ volatile ("movq $5, %%rdi; movq %0, %%rsi; int $0x69" : : "g"(buffer) : "%rdi", "%rsi", "%rax");
}
