#pragma once

#include "syscalls.h"
#include <definitions.h>

static bool checkFolder(const char* name)
{
    SYSCALL_1_RETURN(9, bool, name);
}

static bool checkFile(const char* name)
{
    SYSCALL_1_RETURN(10, bool, name);
}

static void createFolder(const char* name)
{
    SYSCALL_1(11, name);
}

static void* createFile(const char* name, uint64_t size)
{
    SYSCALL_2_RETURN(12, void*, name, size);
}

static const char** getFiles(const char* root, uint64_t* count)
{
    SYSCALL_2_RETURN(13, const char**, root, count);
}

static uint8_t* getFile(const char* name, uint64_t* size)
{
    SYSCALL_2_RETURN(14, uint8_t*, name, size);
}

static void deleteFile(const char* name)
{
    SYSCALL_1(15, name);
}
