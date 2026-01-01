#pragma once

static void put(char character)
{
    __asm__ volatile ("movq $0, %%rdi; movq %0, %%rsi; int $0x69" : : "g"(character) : "%rdi", "%rsi", "%rax");
}

static void write(const char* message)
{
    __asm__ volatile ("movq $1, %%rdi; movq %0, %%rsi; int $0x69" : : "g"(message) : "%rdi", "%rsi", "%rax");
}

static void clear()
{
    __asm__ volatile ("movq $2, %%rdi; int $0x69" : :  : "%rdi", "%rax");
}

static void read(char* buffer)
{
    __asm__ volatile ("movq $3, %%rdi; movq %0, %%rsi; int $0x69" : : "g"(buffer) : "%rdi", "%rsi", "%rax");
}

static void print(const char* string)
{
    put('[');
    write(__builtin_strrchr(__BASE_FILE__, '/') + 1);
    write("]: ");
    write(string);
    put('\n');
}
