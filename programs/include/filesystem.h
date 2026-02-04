#pragma once

#include "syscalls.h"
#include <definitions.h>
#include <calls.h>

static bool checkFolder(const char* name)
{
    SYSCALL_1_RETURN(CHECK_FOLDER, bool, name);
}

static bool checkFile(const char* name)
{
    SYSCALL_1_RETURN(CHECK_FILE, bool, name);
}

static void createFolder(const char* name)
{
    SYSCALL_1(CREATE_FOLDER, name);
}

static void* createFile(const char* name, uint64_t size)
{
    SYSCALL_2_RETURN(CREATE_FILE, void*, name, size);
}

static const char** getFiles(const char* root, uint64_t* count)
{
    SYSCALL_2_RETURN(GET_FILES, const char**, root, count);
}

static uint8_t* getFile(const char* name, uint64_t* size)
{
    SYSCALL_2_RETURN(GET_FILE, uint8_t*, name, size);
}

static void deleteFile(const char* name)
{
    SYSCALL_1(DELETE_FILE, name);
}
