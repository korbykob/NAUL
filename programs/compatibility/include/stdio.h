#pragma once

#include <processes.h>
#include <tty.h>
#include <filesystem.h>

#define NANOPRINTF_USE_FIELD_WIDTH_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_PRECISION_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_FLOAT_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_FLOAT_HEX_FORMAT_SPECIFIER 1
#define NANOPRINTF_USE_SMALL_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_LARGE_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_BINARY_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_WRITEBACK_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_ALT_FORM_FLAG 1
#define NANOPRINTF_USE_FLOAT_SINGLE_PRECISION 0
#define NANOPRINTF_VISIBILITY_STATIC
#define NANOPRINTF_IMPLEMENTATION
#include "nanoprintf.h"

#define exit(status) quit();
#define FILE void
#define fflush(stream) ({ 0; })
#define fprintf(stream, format, ...) printf(format, __VA_ARGS__)
#define vprintf(format, args) npf_vpprintf(nanoPut, 0, format, args)

static struct
{
    uint8_t* memory;
    uint64_t size;
    uint64_t offset;
} openFiles[5];

static void nanoPut(int c, void *ctx)
{
    put(c);
}

static int printf(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    int value = npf_vpprintf(nanoPut, 0, format, args);
    va_end(args);
    return value;
}

static void* fopen(const char* filename, const char* mode)
{
    if (checkFile(filename))
    {
        uint64_t id = 0;
        while (openFiles[id].memory)
        {
            id++;
        }
        openFiles[id].offset = 0;
        openFiles[id].memory = getFile(filename, &openFiles[id].size);
        return (void*)(id + 1);
    }
    return 0;
}

static void fclose(void* handle)
{
    openFiles[(uint64_t)handle - 1].memory = 0;
}

static int fread(void* buf, uint64_t size, uint64_t count, void* handle)
{
    int bytes = min(openFiles[(uint64_t)handle - 1].offset + (size * count), openFiles[(uint64_t)handle - 1].size) - openFiles[(uint64_t)handle - 1].offset;
    uint8_t* source = openFiles[(uint64_t)handle - 1].memory + openFiles[(uint64_t)handle - 1].offset;
    uint8_t* destination = (uint8_t*)buf;
    for (int i = 0; i < bytes; i++)
    {
        *destination++ = *source++;
    }
    openFiles[(uint64_t)handle - 1].offset += bytes;
    return bytes;
}
