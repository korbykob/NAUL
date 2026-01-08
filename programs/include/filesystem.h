#pragma once

#include <definitions.h>

static bool checkFolder(const char* name)
{
    uint64_t result = 0;
    __asm__ volatile ("movq $9, %%rdi; movq %1, %%rsi; int $0x69; movq %%rax, %0" : "=g"(result) : "g"(name) : "%rdi", "%rsi", "%rax");
    return (bool)result;
}

static bool checkFile(const char* name)
{
    uint64_t result = 0;
    __asm__ volatile ("movq $10, %%rdi; movq %1, %%rsi; int $0x69; movq %%rax, %0" : "=g"(result) : "g"(name) : "%rdi", "%rsi", "%rax");
    return (bool)result;
}

static void createFolder(const char* name)
{
    __asm__ volatile ("movq $11, %%rdi; movq %0, %%rsi; int $0x69" : : "g"(name) : "%rdi", "%rsi", "%rax");
}

static void* createFile(const char* name, uint64_t size)
{
    uint64_t result = 0;
    __asm__ volatile ("movq $12, %%rdi; movq %1, %%rsi; movq %2, %%rdx; int $0x69; movq %%rax, %0" : "=g"(result) : "g"(name), "g"(size) : "%rdi", "%rsi", "%rdx", "%rax");
    return (void*)result;
}

static const char** getFiles(const char* root, uint64_t* count)
{
    uint64_t result = 0;
    __asm__ volatile ("movq $13, %%rdi; movq %1, %%rsi; movq %2, %%rdx; int $0x69; movq %%rax, %0" : "=g"(result) : "g"(root), "g"(count) : "%rdi", "%rsi", "%rdx", "%rax");
    return (void*)result;
}

static uint8_t* getFile(const char* name, uint64_t* size)
{
    uint64_t result = 0;
    __asm__ volatile ("movq $14, %%rdi; movq %1, %%rsi; movq %2, %%rdx; int $0x69; movq %%rax, %0" : "=g"(result) : "g"(name), "g"(size) : "%rdi", "%rsi", "%rdx", "%rax");
    return (uint8_t*)result;
}

static void deleteFile(const char* name)
{
    __asm__ volatile ("movq $15, %%rdi; movq %0, %%rsi; int $0x69" : : "g"(name) : "%rdi", "%rsi", "%rax");
}
