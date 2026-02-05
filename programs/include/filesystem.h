#pragma once

#include "syscalls.h"
#include <definitions.h>
#include <calls.h>

static inline bool checkFolder(const char* name)
{
    SYSCALL_1_RETURN(CHECK_FOLDER, bool, name);
}

static inline bool checkFile(const char* name)
{
    SYSCALL_1_RETURN(CHECK_FILE, bool, name);
}

static inline void createFolder(const char* name)
{
    SYSCALL_1(CREATE_FOLDER, name);
}

static inline void* createFile(const char* name, uint64_t size)
{
    SYSCALL_2_RETURN(CREATE_FILE, void*, name, size);
}

static inline const char** getFiles(const char* root, uint64_t* count)
{
    SYSCALL_2_RETURN(GET_FILES, const char**, root, count);
}

static inline uint8_t* getFile(const char* name, uint64_t* size)
{
    SYSCALL_2_RETURN(GET_FILE, uint8_t*, name, size);
}

static inline void deleteFile(const char* name)
{
    SYSCALL_1(DELETE_FILE, name);
}
