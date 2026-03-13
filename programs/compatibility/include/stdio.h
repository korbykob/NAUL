#pragma once

#include <tty.h>
#include <processes.h>

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

#define fflush(stream) 0
#define fprintf(stream, format, ...) printf(format, __VA_ARGS__)

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
